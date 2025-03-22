#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentUnboxingWizard.hpp"
#include "agent/FaceActivity.hpp"
#include "activities/MessageActivity.hpp"
#include "agent/TransitionActivity.hpp"
#include "connection/ConnectionActivity.hpp"
#include "delivery/AgentBaptismActivity.hpp"
#include "delivery/AgentDeliveryActivity.hpp"
#include "delivery/AgentSipAndSeeActivity.hpp"
#include "delivery/IdentityMenuActivity.hpp"
#include "hardware/ControllerActivity.hpp"
#include "hardware/ControllerTypeSelector.hpp"
#include "hardware/HardwareActivity.hpp"
#include "hardware/LobeTypeSelector.hpp"
#include "hardware/SerialDeviceSelector.hpp"
#include "identity/IdentityActivity.hpp"
#include "node/NodeNavigation.hpp"
#include "pairing/PairingActivity.hpp"
#include "pairing/PairingMenuActivity.hpp"
#include "pairing/camera/CameraPairingActivity.hpp"
#include "pairing/network/NetworkPairingActivity.hpp"
#include "pairing/trust/PairingTrustActivity.hpp"
#include "stanza/StanzaEditorActivity.hpp"
#include "stanza/StanzaEditorActivity.hpp"
#include "stanza/StanzaManagerActivity.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QDebug>
#include <QMenu>
#include <QAction>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif


AgentWindow::AgentWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::AgentWindow)
	, mMenu(OC_NEW QMenu(tr("Menu"), this))
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
	OC_METHODGATE();
	auto a = agent();
	if(!a.isNull()) {
		mMessageActivity = QSharedPointer<MessageActivity>::create();
		ui->widgetActivityStack->registerActivity(mMessageActivity, false, false, false);
		
		mTransitionActivity = QSharedPointer<TransitionActivity>::create();
		ui->widgetActivityStack->registerActivity(mTransitionActivity, false, false, false);
		
		mSipAndSeeActivity = QSharedPointer<AgentSipAndSeeActivity>::create();
		mSipAndSeeActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mSipAndSeeActivity, false, false, false);
		
		mBaptismActivity = QSharedPointer<AgentBaptismActivity>::create();
		mBaptismActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mBaptismActivity, false, false, false);
		
		mDeliveryActivity = QSharedPointer<AgentDeliveryActivity>::create();
		mDeliveryActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mDeliveryActivity, false, false, false);
		
		mControllerTypeSelector = QSharedPointer<ControllerTypeSelector>::create();
		mControllerTypeSelector->configure(a);
		ui->widgetActivityStack->registerActivity(mControllerTypeSelector, false, false, false);
		
		mSerialDeviceSelector = QSharedPointer<SerialDeviceSelector>::create();
		mSerialDeviceSelector->configure(a);
		ui->widgetActivityStack->registerActivity(mSerialDeviceSelector, false, false, false);
		
		mLobeTypeSelectorActivity = QSharedPointer<LobeTypeSelector>::create();
		mLobeTypeSelectorActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mLobeTypeSelectorActivity, false, false, false);
		
		mStanzaManagerActivity = QSharedPointer<StanzaManagerActivity>::create();
		mStanzaManagerActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mStanzaManagerActivity, false, false, false);
		
		mStanzaEditorActivity = QSharedPointer<StanzaEditorActivity>::create();
		mStanzaEditorActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mStanzaEditorActivity, false, false, false);
		
		mControllerConfiguration = QSharedPointer<ControllerActivity>::create();
		mControllerConfiguration->configure(a);
		ui->widgetActivityStack->registerActivity(mControllerConfiguration, false, false, false);
		
		mHardwareConfiguration = QSharedPointer<HardwareActivity>::create();
		mHardwareConfiguration->configure(a);
		ui->widgetActivityStack->registerActivity(mHardwareConfiguration, false, false, false);
		
		mIdentityActivity = QSharedPointer<IdentityActivity>::create();
		mIdentityActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mIdentityActivity, false, false, false);
		
		mIdentityMenuActivity = QSharedPointer<IdentityMenuActivity>::create();
		mIdentityMenuActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mIdentityMenuActivity, false, false, false);
		
		mPairingMenuActivity = QSharedPointer<PairingMenuActivity>::create();
		mPairingMenuActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mPairingMenuActivity, false, false, false);
		
		mPairingActivity = QSharedPointer<PairingActivity>::create();
		mPairingActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mPairingActivity, false, false, false);
		
		mNetworkPairingActivity = QSharedPointer<NetworkPairingActivity>::create();
		mNetworkPairingActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mNetworkPairingActivity, true, true, false);
		
		mPairingTrustActivity = QSharedPointer<PairingTrustActivity>::create();
		mPairingTrustActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mPairingTrustActivity, true, true, false);
		
		mCameraPairingActivity = QSharedPointer<CameraPairingActivity>::create();
		mCameraPairingActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mCameraPairingActivity, true, true, false);
		
		mFaceActivity = QSharedPointer<FaceActivity>::create();
		mFaceActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mFaceActivity, true, true, true);
		
		mConnectionActivity = QSharedPointer<ConnectionActivity>::create();
		mConnectionActivity->configure(a);
		ui->widgetActivityStack->registerActivity(mConnectionActivity, true, true, true);
		
		mUnboxingWizard = QSharedPointer<AgentUnboxingWizard>::create();
		mUnboxingWizard->configure(a);
		ui->widgetActivityStack->registerActivity(mUnboxingWizard, true, true, true);
	}
	if(!connect(ui->widgetActivityStack, &ActivityStack::currentPageChanged, this, &AgentWindow::onPageChanged, OC_CONTYPE)){
		qWarning()<<"Could not connect to ActivityStack::currentPageChanged";
	}
	pushPage(mFaceActivity->objectName());
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
			swapPage(mFaceActivity->objectName());
		}
		else{
			mUnboxingWizard->updateStage();
			swapPage(mUnboxingWizard->objectName());
		}
	}
}


void AgentWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	auto a = agent();
	if(a){
		a->log(text);
	}
	if(mDebug){
		qDebug()<<"AGENT-LOG-APPEND: "<<text;
	}
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
	addPage(mFaceActivity, true, true, true);
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


void AgentWindow::notImplementedAction(){
	OC_METHODGATE();
	qWarning()<<"NOT IMPLEMENTED YET";
}


const QString AgentWindow::mQuitApplicationText{R"(
Would you like to quit?
)"};



void AgentWindow::requestApplicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT ASK";
	}
	pushPage("MessageActivity", QStringList() << "quit" << "Quit" << mQuitApplicationText << ":/icons/app/warning.svg" << "true");
}


void AgentWindow::applicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT CONFIRM";
	}
	pushPage("TransitionActivity", QStringList() << "quit");
	emit node()->nodeRequestExit(EXIT_SUCCESS);
}


void AgentWindow::pairing(){
	OC_METHODGATE();
	pushPage("PairingActivity");
}


void AgentWindow::identity(){
	OC_METHODGATE();
	pushPage("IdentityActivity");
}


void AgentWindow::configureHardware(){
	OC_METHODGATE();
	pushPage("HardwareActivity");
}


void AgentWindow::connection(){
	OC_METHODGATE();
	pushPage("ConnectionActivity");
}

void AgentWindow::editPlan(){
	OC_METHODGATE();
	pushPage("PlanEditor");
}


void AgentWindow::toggleOnline(bool online){
	OC_METHODGATE();
	Q_UNUSED(online);
	auto a = agent();
	if(!a.isNull()) {
		
	} else {
		qWarning()<<"WARNING: No Agent";
	}
}


void AgentWindow::face(){
	OC_METHODGATE();
	pushPage("FaceActivity");
}


template <typename MethodType>
QAction* AgentWindow::addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable) {
	OC_METHODGATE();
	QAction *action=OC_NEW QAction(title, this);
	action->setCheckable(checkable);
	if(checkable){
		if(!(connect(action, &QAction::toggled, this, method, OC_CONTYPE))){
			qWarning() << "Could not connect action method for" << action->text() << "to menu";
		}else{
			action->setStatusTip(toolTip);
			action->setIcon(QIcon(icon));
			mMenu->addAction(action);
		}
	}
	else{
		if(!(connect(action, &QAction::triggered, this, method, OC_CONTYPE))){
			qWarning() << "Could not connect action method for" << action->text() << "to menu";
		}else{
			action->setStatusTip(toolTip);
			action->setIcon(QIcon(icon));
			mMenu->addAction(action);
		}
	}
	return action;
}


void AgentWindow::prepareMenu( )
{
	OC_METHODGATE();
	mToggleOnlineAction = addMenuItem(tr("Toggle Online"), ":/icons/general/on.svg", tr("Toggle availability of Agent on networks"), &AgentWindow::toggleOnline, true);
	mIdentityAction = addMenuItem(tr("Identity"), ":/icons/pairing/identity.svg", tr("Manage the identity of this Agent"), &AgentWindow::identity );
	mHardwareAction = addMenuItem(tr("Configuration"), ":/icons/controller/actuator_control.svg", tr("Manage the configuration of this Agent"), &AgentWindow::configureHardware );
	mPairingAction = addMenuItem(tr("Pairing"), ":/icons/pairing/pair.svg", tr("Manage the pairing of this Agent"), &AgentWindow::pairing );
	mConnectionAction = addMenuItem(tr("Connection"), ":/icons/controller/network.svg", tr("Manage Agent connection"), &AgentWindow::connection );
	mShowFaceAction = addMenuItem(tr("Face"), ":/icons/general/on.svg", tr("Show the Agent's face in main screen"), &AgentWindow::face);
	mPlanEditorAction = addMenuItem(tr("Plan Editor"), ":/icons/plan/mandate.svg", tr("Edit Agent plan"), &AgentWindow::editPlan );
	
	
	mQuitAction = addMenuItem(tr("Exit"), ":/icons/app/no.svg", tr("Terminate execution of this Agent"), &AgentWindow::requestApplicationShutdown );
	
	/* Other stuff
	
		mShowOnlineButtonAction->setStatusTip(tr("Show connect button in main screen"));
		mShowOnlineButtonAction->setCheckable(true);
		connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
		mMenu.addAction(mShowOnlineButtonAction);
	// Show Face
	//////////////////
	
	
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
