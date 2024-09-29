#include "AgentWindow.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/Message.hpp"
#include "agent/Quitting.hpp"
#include "ui_AgentWindow.h"

#include "agent/Agent.hpp"
#include "agent/AgentUnboxingWizard.hpp"
#include "agent/FaceWidget.hpp"
#include "app/Constants.hpp"
#include "delivery/AgentDeliveryWizard.hpp"
#include "hardware/HardwareWizard.hpp"
#include "hardware/ControllerTypeSelector.hpp"
#include "hardware/SerialDeviceSelector.hpp"
#include "hardware/ControllerConfiguration.hpp"
#include "node/NodeNavigation.hpp"
#include "pairing/PairingWizard.hpp"
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
	auto pageReallyQuit = OC_NEW Quitting();
	if(!connect(pageReallyQuit, &Quitting::done, this, &AgentWindow::onQuitApplication, OC_CONTYPE)){
		qWarning()<<"Could not connect to Quitting::done";
	}
	ui->widgetActivityStack->registerPage(pageReallyQuit, true, true, true);
	
	mQuitting = OC_NEW Message();
	mQuitting->setMesasge("Quitting...");
	ui->widgetActivityStack->registerPage(mQuitting, false, false, false);
	
	mDelivery = OC_NEW AgentDeliveryWizard();
	ui->widgetActivityStack->registerPage(mDelivery, false, false, false);
	
	//mHardware = OC_NEW HardwareWizard();
	//ui->widgetActivityStack->registerPage(mHardware, false, false, false);
	
	mControllerTypeSelector = OC_NEW ControllerTypeSelector();
	ui->widgetActivityStack->registerPage(mControllerTypeSelector, false, false, false);
	
	mSerialDeviceSelector = OC_NEW SerialDeviceSelector();
	ui->widgetActivityStack->registerPage(mSerialDeviceSelector, false, false, false);
	
	mControllerConfiguration = OC_NEW ControllerConfiguration();
	ui->widgetActivityStack->registerPage(mControllerConfiguration, false, false, false);

	mPairing = OC_NEW PairingWizard();
	ui->widgetActivityStack->registerPage(mPairing, false, false, false);
	
	mFaceWidget = OC_NEW FaceWidget();
	ui->widgetActivityStack->registerPage(mFaceWidget, true, true, true);
	
	mUnboxingWizard = OC_NEW AgentUnboxingWizard();
	ui->widgetActivityStack->registerPage(mUnboxingWizard, true, true, true);
	
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
			mControllerConfiguration->configure(a);
			mControllerTypeSelector->configure(a);
			mDelivery->configure(a);
			mFaceWidget->setAgent(a);
			mPairing->configure(a);
			mSerialDeviceSelector->configure(a);
			mUnboxingWizard->configure(a);
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
			//ui->widgetPairing->configure(node());
			if(!connect(mUnboxingWizard, &AgentUnboxingWizard::nextUnboxingStage, this, &AgentWindow::onNextUnboxingStage, OC_CONTYPE)){
				qWarning()<<"Could not connect to AgentUnboxingWizard::nextUnboxingStage";
			}
			if(!connect(mPairing, &PairingWizard::done, this, &AgentWindow::unboxingWizardDone, OC_CONTYPE)){
				qWarning()<<"Could not connect to PairingWizard::done";
			}
			if(!connect(mControllerTypeSelector, &ControllerTypeSelector::done, this, &AgentWindow::controllerTypeSelected, OC_CONTYPE)){
				qWarning()<<"Could not connect to ControllerTypeSelector::done";
			}
			if(!connect(mSerialDeviceSelector, &SerialDeviceSelector::done, this, &AgentWindow::serialControllerSelected, OC_CONTYPE)){
				qWarning()<<"Could not connect to SerialDeviceSelector::done";
			}
			if(!connect(mControllerConfiguration, &ControllerConfiguration::done, this, &AgentWindow::unboxingWizardDone, OC_CONTYPE)){
				qWarning()<<"Could not connect to ControllerConfiguration::done";
			}
			unboxingWizardDone();

		} else {
			qWarning()<<"WARNING: No Agent in agent window configure";
		}
	}
}


void AgentWindow::unboxingWizardDone(){
	OC_METHODGATE();
	if(mUnboxingWizard->unboxingDone()){
		swapPage(mFaceWidget->objectName());
	}
	else{
		mUnboxingWizard->updateStage();
		swapPage(mUnboxingWizard->objectName());
	}
}


void AgentWindow::onNextUnboxingStage(UnboxingStage stage){
	OC_METHODGATE();
	switch(stage){
		case CONTROLLER_CONFIGURATION_STAGE: swapPage(mControllerConfiguration->objectName()); break;
		case CONTROLLER_TYPE_STAGE: swapPage(mControllerTypeSelector->objectName()); break;
		case DELIVERY_STAGE: swapPage(mDelivery->objectName()); break;
		case PAIRING_STAGE: swapPage(mPairing->objectName()); break;
		case SERIAL_DEVICE_STAGE: swapPage(mSerialDeviceSelector->objectName()); break;
		case UNBOXING_COMPLETE: swapPage(mFaceWidget->objectName()); break;
		default:
		case UNKNOWN_STAGE: swapPage(mUnboxingWizard->objectName()); break;
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
	qDebug()<<"AGENT-LOG-APPEND: "<<text;
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
	qWarning()<<"POP PAGE " << ret;
	return ret;
}


bool AgentWindow::pushPage(const QString &title){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->push(title);
	qWarning()<<"PUSH PAGE " << title << ret;
	return ret;
}


QString AgentWindow::swapPage(const QString &title){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->swap(title);
	qWarning()<<"SWAP PAGE " << ret << "-->" << title;
	return ret;
}


void AgentWindow::navigateBack(){
	OC_METHODGATE();
	qDebug() << "BACK";
	popPage();
}


void AgentWindow::openMenu(){
	OC_METHODGATE();
	qWarning()<<"MENU";
	mMenu->exec(mapToGlobal(ui->widgetNavigation->menuPos()));
}


void AgentWindow::onNotImplementedYet(){
	OC_METHODGATE();
	qWarning()<<"NOT IMPLEMENTED YET";
}


void AgentWindow::onStartQuitApplication(){
	OC_METHODGATE();
	qWarning()<<"QUIT ASK";
	pushPage("Quitting");
}


void AgentWindow::onQuitApplication(bool quitting){
	OC_METHODGATE();
	if(quitting){
		qWarning()<<"QUIT CONFIRM";
		pushPage("Quitting");
		emit node()->nodeRequestExit(EXIT_SUCCESS);
	}
	else{
		qWarning()<<"QUIT CANCEL";
		popPage();
	}
}


void AgentWindow::onStartCameraPairing(){
	OC_METHODGATE();
	pushPage("CameraPairing");
}


void AgentWindow::onStartNormalPairing(){
	OC_METHODGATE();
	pushPage("PairingWizard");
}


void AgentWindow::onConfigureHardware(){
	OC_METHODGATE();
	pushPage("HardwareWizard");
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
		qWarning()<<"Could not connect action method for"<<action->text()<<"to menu";
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
	qDebug()<<"Controller type selected" << type;
	auto a = agent();
	auto configStore = a->configurationStore();
	if(!configStore.isNull()) {
		auto config = configStore->agentConfig();
		if(!config.isNull()) {
			config->setControllerName(type);
			qDebug() << "Stored controller type" << type;
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
	qDebug()<<"Serial Controller selected" << controller;
	unboxingWizardDone();
}
