#include "AgentWindow.hpp"
#include "agent/TransitionActivity.hpp"
#include "delivery/AgentBaptismActivity.hpp"
#include "delivery/AgentSipAndSeeActivity.hpp"
#include "pairing/PairingTrustActivity.hpp"
#include "ui_AgentWindow.h"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentUnboxingWizard.hpp"
#include "agent/FaceWidget.hpp"
#include "agent/MessageActivity.hpp"
#include "app/Constants.hpp"
#include "delivery/AgentDeliveryActivity.hpp"
#include "hardware/ControllerActivity.hpp"
#include "hardware/ControllerTypeSelector.hpp"
#include "hardware/SerialDeviceSelector.hpp"
#include "node/NodeNavigation.hpp"
#include "pairing/PairingActivity.hpp"
#include "pairing/CameraPairingActivity.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QDebug>
#include <QMenu>
#include <QAction>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif


AgentWindow::AgentWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::AgentWindow)
	, mMenu(OC_NEW QMenu(tr("Menu"), this))
	, mQuitAction{nullptr}
	, mStartCameraPairingAction{nullptr}
	, mStartNormalPairingAction{nullptr}
	, mHardwareAction{nullptr}
	, mPlanEditorAction{nullptr}
	, mToggleOnlineAction{nullptr}
	
	, mShowFaceAction{nullptr}
	, mConfigureHelper("AgentWindow", true, false, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
	mWaterMark.load(QString(":/images/agent_watermark.svg"));
	prepareMenu();
	prepareNavigation();
}


AgentWindow::~AgentWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui = nullptr;
}


QSharedPointer<Agent> AgentWindow::agent()
{
	OC_METHODGATE();
	QSharedPointer<Node> n = node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in agent()";
	}
	return qSharedPointerCast<Agent> (n);
}


void AgentWindow::prepareActivities(){
	OC_METHODGATE();
	auto a = agent();
	if(!a.isNull()) {
		mMessageActivity = OC_NEW MessageActivity();
		ui->widgetActivityStack->registerPage(mMessageActivity, false, false, false);
		
		mTransitionActivity = OC_NEW TransitionActivity();
		ui->widgetActivityStack->registerPage(mTransitionActivity, false, false, false);
		
		mSipAndSeeActivity = OC_NEW AgentSipAndSeeActivity();
		mSipAndSeeActivity->configure(a);
		ui->widgetActivityStack->registerPage(mSipAndSeeActivity, false, false, false);
		
		mBaptismActivity = OC_NEW AgentBaptismActivity();
		mBaptismActivity->configure(a);
		ui->widgetActivityStack->registerPage(mBaptismActivity, false, false, false);
		
		mDeliveryActivity = OC_NEW AgentDeliveryActivity();
		mDeliveryActivity->configure(a);
		ui->widgetActivityStack->registerPage(mDeliveryActivity, false, false, false);
		
		mControllerTypeSelector = OC_NEW ControllerTypeSelector();
		mControllerTypeSelector->configure(a);
		ui->widgetActivityStack->registerPage(mControllerTypeSelector, false, false, false);
		
		mSerialDeviceSelector = OC_NEW SerialDeviceSelector();
		mSerialDeviceSelector->configure(a);
		ui->widgetActivityStack->registerPage(mSerialDeviceSelector, false, false, false);
		
		mControllerConfiguration = OC_NEW ControllerActivity();
		mControllerConfiguration->configure(a);
		ui->widgetActivityStack->registerPage(mControllerConfiguration, false, false, false);
		
		mPairingActivity = OC_NEW PairingActivity();
		mPairingActivity->configure(a);
		ui->widgetActivityStack->registerPage(mPairingActivity, false, false, false);
		
		mPairingTrustActivity = OC_NEW PairingTrustActivity();
		mPairingTrustActivity->configure(a);
		ui->widgetActivityStack->registerPage(mPairingTrustActivity, true, true, false);
		
		
		mCameraPairingActivity = OC_NEW CameraPairingActivity();
		mCameraPairingActivity->configure(a);
		ui->widgetActivityStack->registerPage(mCameraPairingActivity, true, true, false);
		
		mFaceWidget = OC_NEW FaceWidget();
		mFaceWidget->configure(a);
		ui->widgetActivityStack->registerPage(mFaceWidget, true, true, true);
		
		mUnboxingWizard = OC_NEW AgentUnboxingWizard();
		mUnboxingWizard->configure(a);
		ui->widgetActivityStack->registerPage(mUnboxingWizard, true, true, true);
	}
	if(!connect(ui->widgetActivityStack, &ActivityStack::currentPageChanged, this, &AgentWindow::onPageChanged, OC_CONTYPE)){
		qWarning()<<"Could not connect to ActivityStack::currentPageChanged";
	}
	pushPage(mFaceWidget->objectName());
}


void AgentWindow::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		auto a = agent();
		if(!a.isNull()) {
			// [...]
			loadWindowGeometry();
			
			prepareActivities();
			ui->widgetNavigation->configure(node());
			
			updateWindowIcon();
			auto s = a->settings();
			if(!s.isNull()) {
				mShowFaceAction->setChecked(s->getCustomSettingBool(Constants::AGENT_FACE_EYES_SHOW));
				
				//mOnlineAction->setChecked(s->getCustomSettingBool(Constants::AGENT_CONNECTION_STATUS, false));
				//mShowOnlineButtonAction->setChecked(s->getCustomSettingBool(Constants::AGENT_CONNECTION_STATUS));
				//mShowLogAction->setChecked(s->getCustomSettingBool(Constants::AGENT_FACE_LOG_SHOW));
				//mShowStatsAction->setChecked(s->getCustomSettingBool(Constants::AGENT_FACE_STATS_SHOW));
			}
			//updateOnlineStatus();
			updateWindowIcon();

			unboxingWizardDone();

		} else {
			qWarning()<<"WARNING: No Agent in agent window configure";
		}
	}
}


void AgentWindow::unboxingWizardDone(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mUnboxingWizard->unboxingDone()){
			swapPage(mFaceWidget->objectName());
		}
		else{
			mUnboxingWizard->updateStage();
			swapPage(mUnboxingWizard->objectName());
		}
	}
}


void AgentWindow::updateFaceVisibility()
{
	OC_METHODGATE();
	mFaceWidget->updateVisibility();
}


void AgentWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"AGENT-LOG-APPEND: "<<text;
	}
	mFaceWidget->appendLog(text);
}


//////////////////


void AgentWindow::prepareNavigation(){
	OC_METHODGATE();
	if(!connect(ui->widgetNavigation, &NodeNavigation::navigateBack, this, &AgentWindow::navigateBack, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation back button";
	}
	if(!connect(ui->widgetNavigation, &NodeNavigation::openMenu, this, &AgentWindow::openMenu, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation menu button";
	}
	ui->widgetNavigation->setState("OctoMY™ Agent", true, true, true);
	/*
	addPage(ui->pageConfirmQuit, true, true, false);
	addPage(ui->pageQuitting, false, false, false);
	addPage(mFaceWidget, true, true, true);
	addPage(ui->pageDelivery, false, false, false);
	addPage(ui->pageHardware, false, false, false);
	addPage(ui->pagePairing, false, false, false);
	addPage(ui->pageUnboxing, false, false, false);
	setCurrentPage("pageFace");
*/
}


QString AgentWindow::popPage(){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->pop();
	if(mDebug){
		qDebug()<<"POP PAGE " << ret;
	}
	return ret;
}


bool AgentWindow::pushPage(const QString &title, const QStringList arguments){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->push(title, arguments);
	if(mDebug){
		qDebug()<<"PUSH PAGE " << title << arguments << ret;
	}
	return ret;
}


QString AgentWindow::swapPage(const QString &title){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->swap(title);
	if(mDebug){
		qDebug()<<"SWAP PAGE " << ret << "-->" << title;
	}
	return ret;
}


void AgentWindow::navigateBack(){
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "BACK";
	}
	popPage();
}


void AgentWindow::openMenu(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"MENU";
	}
	mMenu->exec(mapToGlobal(ui->widgetNavigation->menuPos()));
}


void AgentWindow::onNotImplementedYet(){
	OC_METHODGATE();
	qWarning()<<"NOT IMPLEMENTED YET";
}


const QString AgentWindow::mQuitApplicationText{R"(
Would you like to quit?
)"};



void AgentWindow::onStartQuitApplication(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT ASK";
	}
	pushPage("MessageActivity", QStringList() << "quit" << "Quit" << mQuitApplicationText << ":/icons/warning.svg" << "true");
}


void AgentWindow::quitApplication(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT CONFIRM";
	}
	pushPage("TransitionActivity", QStringList() << "quit");
	emit node()->nodeRequestExit(EXIT_SUCCESS);
}


void AgentWindow::onStartCameraPairing(){
	OC_METHODGATE();
	pushPage("CameraPairing");
}


void AgentWindow::onStartNormalPairing(){
	OC_METHODGATE();
	pushPage("PairingActivity");
}


void AgentWindow::onConfigureHardware(){
	OC_METHODGATE();
	pushPage("ControllerActivity");
}


void AgentWindow::onPlanEditor(){
	OC_METHODGATE();
	pushPage("PlanEditor");
}


void AgentWindow::onOnlineChanged(){
	OC_METHODGATE();
	auto a = agent();
	if(!a.isNull()) {
		
	} else {
		qWarning()<<"WARNING: No Agent";
	}
}


void AgentWindow::onFaceVisibilityChanged(){
	OC_METHODGATE();
	onNotImplementedYet();
}


QAction *AgentWindow::addMenuItem(QString title, QString icon, QString toolTip, void (AgentWindow::*method)() , bool checkable){
	OC_METHODGATE();
	QAction *action=OC_NEW QAction(title, this);
	action->setCheckable(checkable);
	if(!(connect(action, &QAction::triggered, this, method, OC_CONTYPE))){
		qWarning() << "Could not connect action method for" << action->text() << "to menu";
	}else{
		action->setStatusTip(toolTip);
		action->setIcon(QIcon(icon));
		mMenu->addAction(action);
	}
	return action;
}


void AgentWindow::prepareMenu( )
{
	OC_METHODGATE();
	mQuitAction = addMenuItem(tr("Exit"), ":/icons/no.svg", tr("Terminate execution of this Agent"), &AgentWindow::onStartQuitApplication );
	mStartCameraPairingAction = addMenuItem(tr("Camera Pairing"), ":/icons/eye.svg", tr("Start Camera Pairing"), &AgentWindow::onStartCameraPairing );
	mStartNormalPairingAction = addMenuItem(tr("Normal Pairing"), ":/icons/pair.svg", tr("Start Normal Pairing"), &AgentWindow::onStartNormalPairing );
	mHardwareAction = addMenuItem(tr("Configure Hardware"), ":/icons/actuator_control.svg", tr("Connect to the body of the robot"), &AgentWindow::onConfigureHardware );
	mPlanEditorAction = addMenuItem(tr("Plan Editor"), ":/icons/mandate.svg", tr("Open the Plan editor"), &AgentWindow::onPlanEditor );
	mToggleOnlineAction = addMenuItem(tr("Go Online"), ":/icons/on.svg", tr("Toggle availability of Agent"), &AgentWindow::onOnlineChanged, true);
	mShowFaceAction = addMenuItem(tr("Show face"), ":/icons/on.svg", tr("Show the Agent's face in main screen"), &AgentWindow::onFaceVisibilityChanged, true);
	
	/* Other stuff
	
		mShowOnlineButtonAction->setStatusTip(tr("Show connect button in main screen"));
		mShowOnlineButtonAction->setCheckable(true);
		connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
		mMenu.addAction(mShowOnlineButtonAction);
	// Show Face
	//////////////////
	
	
	// Show Log scroll
	//////////////////
	mShowLogAction->setStatusTip(tr("Show log in main screen"));
	mShowLogAction->setCheckable(true);
	connect(mShowLogAction, &QAction::triggered, this, &AgentWindow::onLogVisibilityChanged);
	mMenu.addAction(mShowLogAction);
	
	// Show Stats
	//////////////////
	mShowStatsAction->setStatusTip(tr("Show stats in main screen"));
	mShowStatsAction->setCheckable(true);
	connect(mShowStatsAction, &QAction::triggered, this, &AgentWindow::onStatsVisibilityChanged);
	mMenu.addAction(mShowStatsAction);
	
	// Show birth certificate
	///////////////////////////
	mShowBirthCertificateAction->setStatusTip(tr("Show the birth certificate of this agent"));
	mShowBirthCertificateAction->setIcon(QIcon(":/icons/certificate.svg"));
	connect(mShowBirthCertificateAction, &QAction::triggered, this, &AgentWindow::onStartShowBirthCertificate);
	mMenu.addAction(mShowBirthCertificateAction);
	
	// Unbirth
	//////////////////
	mUnbirthAction->setStatusTip(tr("Delete the identity of this agent to restart birth"));
	mUnbirthAction->setIcon(QIcon(":/icons/kill.svg"));
	connect(mUnbirthAction, &QAction::triggered, this, &AgentWindow::onStartUnbirth);
	mMenu.addAction(mUnbirthAction);
	*/
}


void AgentWindow::onPageChanged(const StackPage &page){
	ui->widgetNavigation->setState(page.widget->objectName(), page.headerEnabled, page.backEnabled, page.menuEnabled);
}


void AgentWindow::controllerTypeSelected(const QString &type){
	if(mDebug){
		qDebug() << "Controller type selected" << type;
	}
	auto a = agent();
	auto configStore = a->configurationStore();
	if(!configStore.isNull()) {
		auto config = configStore->agentConfig();
		if(!config.isNull()) {
			config->setControllerName(type);
			configStore->synchronize([=](QSharedPointer<SimpleDataStore> store, bool status){
				Q_UNUSED(store);
				if(mDebug){
					qDebug() << "Stored controller type" << type << status;
				}
			});

		}
		else{
			qWarning() << "Could not stire controller type, no config";
		}
	}
	else{
		qWarning() << "Could not stire controller type, no store";
	}
	unboxingWizardDone();
}


void AgentWindow::serialControllerSelected(const QString &controller){
	if(mDebug){
		qDebug()<<"Serial Controller selected" << controller;
	}
	unboxingWizardDone();
}
