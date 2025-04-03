#include "HubWindow.hpp"

#include "ui_HubWindow.h"

// TODO: Relocate the activities that are clearly malplaced
#include "activities/ActuatorActivity.hpp"
#include "activities/MediaDeviceActivity.hpp"
#include "activities/construct/ConstructActivity.hpp"
#include "activities/ComputeActivity.hpp"
#include "activities/GaitActivity.hpp"
#include "activities/HUDActivity.hpp"
#include "activities/InternalResourcesActivity.hpp"
#include "activities/LogActivity.hpp"
#include "activities/MapActivity.hpp"
#include "activities/MarkdownActivity.hpp"
#include "activities/MessageActivity.hpp"
#include "activities/NetworkUtilsActivity.hpp"
#include "activities/NodesManagementActivity.hpp"
#include "activities/PKIManagerActivity.hpp"
#include "activities/PlanActivity.hpp"
#include "activities/QRUtilityActivity.hpp"
#include "activities/SerialActivity.hpp"
#include "activities/SplitterActivity.hpp"
#include "activities/TriggerManagerActivity.hpp"
#include "activities/VoiceActivity.hpp"
#include "activities/WidgetIllustrationActivity.hpp"
#include "agent/TransitionActivity.hpp"
#include "comms/CommsChannel.hpp"
#include "connection/ConnectionActivity.hpp"
#include "delivery/ControlBaptismActivity.hpp"
#include "delivery/ControlDeliveryActivity.hpp"
#include "delivery/ControlSipAndSeeActivity.hpp"
#include "delivery/IdentityMenuActivity.hpp"
#include "hub/Hub.hpp"
#include "hub/HubMainMenu.hpp"
#include "identity/IdentityActivity.hpp"
#include "navigation/DevelopmentMenuActivity.hpp"
#include "navigation/UtilitiesMenuActivity.hpp"
#include "pairing/PairingActivity.hpp"
#include "pairing/PairingMenuActivity.hpp"
#include "pairing/trust/PairingTrustActivity.hpp"
#include "remote/AgentSelectActivity.hpp"
#include "remote/ControlUnboxingWizard.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QHostInfo>
#include <QMenu>
#include <QNetworkInterface>
#include <QProcess>
#include <QScrollBar>
#include <QString>
#include <QStringList>
#include <QTextStream>


HubWindow::HubWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::HubWindow)
	, mMenu(OC_NEW QMenu(tr("Menu"), this))
	, mConfigureHelper("HubWindow", true, false, false, true)

{
	OC_METHODGATE();
	setObjectName("HubWindow");
	ui->setupUi(this);
}


HubWindow::~HubWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui=nullptr;
}


void HubWindow::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		auto h=hub();
		if(!h.isNull()) {
			loadWindowGeometry();
			prepareNavigation();
			prepareActivities();
			ui->widgetNavigation->configure(node());
			updateWindowIcon();
			unboxingWizardDone();
			log("HUB READY");
		} else {
			qWarning()<<"WARNING: No Agent in agent window configure";
		}
	}
}


QSharedPointer<Hub> HubWindow::hub() const
{
	OC_METHODGATE();
	QSharedPointer<Node> n = node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in hub()";
	}
	return qSharedPointerCast<Hub> (n);
}



template <typename MethodType>
QAction* HubWindow::addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable) {
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



void HubWindow::prepareActivities(){
	OC_METHODGATE();
	auto h = hub();
	if(!h.isNull()) {
		mMessage = QSharedPointer<MessageActivity>::create();
		ui->widgetActivityStack->registerActivity(mMessage, false, false, false);
		
		mTransitionActivity = QSharedPointer<TransitionActivity>::create();
		ui->widgetActivityStack->registerActivity(mTransitionActivity, false, false, false);
		
		mSipAndSeeActivity = QSharedPointer<ControlSipAndSeeActivity>::create();
		mSipAndSeeActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mSipAndSeeActivity, false, false, false);
		
		mBaptismActivity = QSharedPointer<ControlBaptismActivity>::create();
		mBaptismActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mBaptismActivity, false, false, false);
		
		mDeliveryActivity = QSharedPointer<ControlDeliveryActivity>::create();
		mDeliveryActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mDeliveryActivity, false, false, false);
		
		mIdentityActivity = QSharedPointer<IdentityActivity>::create();
		mIdentityActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mIdentityActivity, false, false, false);
		
		mPairingActivity = QSharedPointer<PairingActivity>::create();
		mPairingActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mPairingActivity, false, false, false);
		
		mPairingTrustActivity = QSharedPointer<PairingTrustActivity>::create();
		mPairingTrustActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mPairingTrustActivity, true, true, false);
		
		mConnectionActivity = QSharedPointer<ConnectionActivity>::create();
		mConnectionActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mConnectionActivity, true, true, true);
		
		
		mAgentSelectActivity = QSharedPointer<AgentSelectActivity>::create();
		mAgentSelectActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mAgentSelectActivity, true, true, false);
		
		
		mPKIManagerActivity = QSharedPointer<PKIManagerActivity>::create();
		//mPKIManagerActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mPKIManagerActivity, true, true, false);
		
		
		mQRUtilityActivity = QSharedPointer<QRUtilityActivity>::create();
		//mQRUtilityActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mQRUtilityActivity, true, true, false);
		
		
		mTriggerManagerActivity = QSharedPointer<TriggerManagerActivity>::create();
		mTriggerManagerActivity->configure();
		ui->widgetActivityStack->registerActivity(mTriggerManagerActivity, true, true, true);
		
		mGaitActivity = QSharedPointer<GaitActivity>::create();
		mGaitActivity->configure();
		ui->widgetActivityStack->registerActivity(mGaitActivity, true, true, true);
		
		mMediaDeviceActivity = QSharedPointer<MediaDeviceActivity>::create();
		mMediaDeviceActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mMediaDeviceActivity, true, true, true);
		
		mConstructActivity = QSharedPointer<ConstructActivity>::create();
		mConstructActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mConstructActivity, true, true, true);
		
		mPlanActivity = QSharedPointer<PlanActivity>::create();
		mPlanActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mPlanActivity, true, true, true);
		
		mMapActivity = QSharedPointer<MapActivity>::create();
		mMapActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mMapActivity, true, true, true);
		
		mHUDActivity = QSharedPointer<HUDActivity>::create();
		mHUDActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mHUDActivity, true, true, true);
		
		mActuatorActivity = QSharedPointer<ActuatorActivity>::create();
		mActuatorActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mActuatorActivity, true, true, true);
		
		mWidgetIllustrationActivity = QSharedPointer<WidgetIllustrationActivity>::create();
		mWidgetIllustrationActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mWidgetIllustrationActivity, true, true, true);
		
		mSerialActivity = QSharedPointer<SerialActivity>::create();
		mSerialActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mSerialActivity, true, true, true);
		
		mComputeActivity = QSharedPointer<ComputeActivity>::create();
		mComputeActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mComputeActivity, true, true, true);
		
		mVoiceActivity = QSharedPointer<VoiceActivity>::create();
		mVoiceActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mVoiceActivity, true, true, true);
		
		mMarkdownActivity = QSharedPointer<MarkdownActivity>::create();
		mMarkdownActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mMarkdownActivity, true, true, true);
		
		mLogActivity = QSharedPointer<LogActivity>::create();
		mLogActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mLogActivity, true, true, true);
		
		mSplitterActivity = QSharedPointer<SplitterActivity>::create();
		mSplitterActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mSplitterActivity, true, true, true);
		
		mNetworkUtilsActivity = QSharedPointer<NetworkUtilsActivity>::create();
		mNetworkUtilsActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mNetworkUtilsActivity, true, true, true);
		
		mNodesManagementActivity = QSharedPointer<NodesManagementActivity>::create();
		mNodesManagementActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mNodesManagementActivity, true, true, true);
		
		mInternalResourcesActivity = QSharedPointer<InternalResourcesActivity>::create();
		mInternalResourcesActivity->configure();
		ui->widgetActivityStack->registerActivity(mInternalResourcesActivity, true, true, true);
		
		
		// Menus
		mMainMenu = QSharedPointer<HubMainMenu>::create();
		mMainMenu->configure(h);
		ui->widgetActivityStack->registerActivity(mMainMenu, true, true, true);
		
		mDevelopmentMenu = QSharedPointer<DevelopmentMenuActivity>::create();
		mDevelopmentMenu->configure(h);
		ui->widgetActivityStack->registerActivity(mDevelopmentMenu, true, true, true);
		
		mUtilitiesMenu = QSharedPointer<UtilitiesMenuActivity>::create();
		mUtilitiesMenu->configure(h);
		ui->widgetActivityStack->registerActivity(mUtilitiesMenu, true, true, true);
		
		mIdentityMenuActivity = QSharedPointer<IdentityMenuActivity>::create();
		mIdentityMenuActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mIdentityMenuActivity, false, false, false);
		
		mPairingMenuActivity = QSharedPointer<PairingMenuActivity>::create();
		mPairingMenuActivity->configure(h);
		ui->widgetActivityStack->registerActivity(mPairingMenuActivity, false, false, false);
		
		mUnboxingWizard = QSharedPointer<ControlUnboxingWizard>::create();
		mUnboxingWizard->configure(h);
		ui->widgetActivityStack->registerActivity(mUnboxingWizard, true, true, true);
	}
	if(!connect(ui->widgetActivityStack, &ActivityStack::currentPageChanged, this, &HubWindow::onPageChanged, OC_CONTYPE)){
		qWarning()<<"Could not connect to ActivityStack::currentPageChanged";
	}
	pushPage(mMainMenu->activityName());
}


void HubWindow::prepareNavigation(){
	OC_METHODGATE();
	prepareNavigationMenu();
	if(!connect(ui->widgetNavigation, &NodeNavigation::navigateBack, this, &HubWindow::navigateBack, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation back button";
	}
	if(!connect(ui->widgetNavigation, &NodeNavigation::openMenu, this, &HubWindow::openNavigation, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation menu button";
	}
	ui->widgetNavigation->setState("OctoMY™ Hub", true, true, true);
}


void HubWindow::prepareNavigationMenu()
{
	OC_METHODGATE();
	mToggleOnlineAction = addMenuItem(tr("Toggle Online"), ":/icons/general/on.svg", tr("Toggle availability of Agent on networks"), &HubWindow::toggleOnline, true);
	mIdentityAction = addMenuItem(tr("Identity"), ":/icons/pairing/identity.svg", tr("Manage the identity of this Remote"), &HubWindow::identity );
	mPairingAction = addMenuItem(tr("Pairing"), ":/icons/pairing/pair.svg", tr("Manage the pairing of this Remote"), &HubWindow::pairing );
	mConnectionAction = addMenuItem(tr("Connection"), ":/icons/controller/network.svg", tr("Manage Remote connection"), &HubWindow::connection );
	mMenuAction = addMenuItem(tr("Main Menu"), ":/icons/app/menu.svg", tr("Show the main menu screen"), &HubWindow::menu);
	mQuitAction = addMenuItem(tr("Exit"), ":/icons/general/on.svg", tr("Terminate execution of this Remote"), &HubWindow::requestApplicationShutdown );
}


void HubWindow::log(const QString &text, LogType type) const
{
	OC_METHODGATE();
	auto h=hub();
	if(h){
		h->log(text, type);
	}
}


void HubWindow::applicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT CONFIRM";
	}
	pushPage("TransitionActivity", QStringList() << "quit");
	emit node()->nodeRequestExit(EXIT_SUCCESS);
}


QString HubWindow::popPage(){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->pop();
	qWarning()<<"POP PAGE " << ret;
	return ret;
}


bool HubWindow::pushPage(const QString &title, const QStringList arguments){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->push(title, arguments);
	if(mDebug){
		qDebug()<<"PUSH PAGE " << title << arguments << ret;
	}
	return ret;
}


QString HubWindow::swapPage(const QString &title){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->swap(title);
	qWarning()<<"SWAP PAGE " << ret << "-->" << title;
	return ret;
}

///////////////////////////////////////////



void HubWindow::navigateBack(){
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "BACK";
	}
	popPage();
}


void HubWindow::openNavigation(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"MENU";
	}
	mMenu->exec(mapToGlobal(ui->widgetNavigation->menuPos()));
}


void HubWindow::requestApplicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT ASK";
	}
	pushPage("MessageActivity", QStringList() << "quit" << "Quit" << "Would you like to quit?" << ":/icons/app/warning.svg" << "true");
}


void HubWindow::onPageChanged(const StackPage &page){
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->widgetNavigation->setState(page.widget->activityName(), page.headerEnabled, page.backEnabled, page.menuEnabled);
	}
}


void HubWindow::unboxingWizardDone(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mUnboxingWizard->unboxingDone()){
			swapPage(mMainMenu->activityName());
		}
		else{
			mUnboxingWizard->updateStage();
			swapPage(mUnboxingWizard->activityName());
		}
	}
}


void HubWindow::unboxing(){
	OC_METHODGATE();
	pushPage(mUnboxingWizard->activityName());
}


void HubWindow::pairing(){
	OC_METHODGATE();
	pushPage(mPairingActivity->activityName());
	//pushPage(mPairingMenuActivity->activityName());
}


void HubWindow::connection(){
	OC_METHODGATE();
	pushPage(mConnectionActivity->activityName());
}


void HubWindow::menu(){
	OC_METHODGATE();
	pushPage(mMainMenu->activityName());
}


void HubWindow::identity(){
	OC_METHODGATE();
	pushPage(mIdentityActivity->activityName());
}


void HubWindow::toggleOnline(bool online){
	OC_METHODGATE();
	Q_UNUSED(online);
	auto h = hub();
	if(!h.isNull()) {
	} else {
		qWarning()<<"WARNING: No Hub";
	}
}

