#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"


#include "activities/MessageActivity.hpp"
#include "agent/TransitionActivity.hpp"
#include "connection/ConnectionActivity.hpp"
#include "delivery/ControlBaptismActivity.hpp"
#include "delivery/ControlDeliveryActivity.hpp"
#include "delivery/ControlSipAndSeeActivity.hpp"
#include "delivery/IdentityMenuActivity.hpp"
#include "identity/IdentityActivity.hpp"
#include "pairing/PairingActivity.hpp"
#include "pairing/PairingMenuActivity.hpp"
#include "pairing/trust/PairingTrustActivity.hpp"
#include "remote/AgentSelectActivity.hpp"
#include "remote/ControlUnboxingWizard.hpp"
#include "remote/Remote.hpp"
#include "remote/RemoteController.hpp"
#include "remote/RemoteMainMenu.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QMenu>
#include <QAction>


#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

RemoteWindow::RemoteWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::RemoteWindow)
	, mConfigureHelper("RemoteWindow", true, false, false, true)
{
	OC_METHODGATE();
	setObjectName("RemoteWindow");
	ui->setupUi(this);
	setActivityStack(ui->widgetActivityStack);
}


RemoteWindow::~RemoteWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui = nullptr;
}



void RemoteWindow::log(const QString &text, LogType type) const
{
	OC_METHODGATE();
	auto r = remote();
	if(!r.isNull()) {
		r->log(text, type);
	}
}



QSharedPointer<Remote> RemoteWindow::remote() const
{
	OC_METHODGATE();
	QSharedPointer<Node> n = node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in remote()";
	}

	return qSharedPointerCast<Remote> (n);
}


void RemoteWindow::prepareActivities(){
	auto r = remote();
	if(!r.isNull()) {
		mMessage = QSharedPointer<MessageActivity>::create();
		ui->widgetActivityStack->registerActivity(mMessage, false, false, false);
		
		mTransitionActivity = QSharedPointer<TransitionActivity>::create();
		ui->widgetActivityStack->registerActivity(mTransitionActivity, false, false, false);
		
		mSipAndSeeActivity = QSharedPointer<ControlSipAndSeeActivity>::create();
		mSipAndSeeActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mSipAndSeeActivity, false, false, false);
		
		mBaptismActivity = QSharedPointer<ControlBaptismActivity>::create();
		mBaptismActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mBaptismActivity, false, false, false);
		
		mDeliveryActivity = QSharedPointer<ControlDeliveryActivity>::create();
		mDeliveryActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mDeliveryActivity, false, false, false);
		
		mIdentityActivity = QSharedPointer<IdentityActivity>::create();
		mIdentityActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mIdentityActivity, false, false, false);
		
		mPairingActivity = QSharedPointer<PairingActivity>::create();
		mPairingActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mPairingActivity, false, false, false);
		
		mPairingTrustActivity = QSharedPointer<PairingTrustActivity>::create();
		mPairingTrustActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mPairingTrustActivity, true, true, false);
		
		mConnectionActivity = QSharedPointer<ConnectionActivity>::create();
		mConnectionActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mConnectionActivity, true, true, true);
		
		mAgentSelectActivity = QSharedPointer<AgentSelectActivity>::create();
		mAgentSelectActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mAgentSelectActivity, true, true, false);
		
		mControl = QSharedPointer<RemoteController>::create();
		mControl->configure(r);
		ui->widgetActivityStack->registerActivity(mControl, true, true, true);
		
		mUnboxingWizard = QSharedPointer<ControlUnboxingWizard>::create();
		mUnboxingWizard->configure(r);
		ui->widgetActivityStack->registerActivity(mUnboxingWizard, true, true, true);
		
		
		// Menus
		mMainMenu = QSharedPointer<RemoteMainMenu>::create();
		mMainMenu->configure(r);
		ui->widgetActivityStack->registerActivity(mMainMenu, true, true, true);
		
		mIdentityMenuActivity = QSharedPointer<IdentityMenuActivity>::create();
		mIdentityMenuActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mIdentityMenuActivity, false, false, false);
		
		mPairingMenuActivity = QSharedPointer<PairingMenuActivity>::create();
		mPairingMenuActivity->configure(r);
		ui->widgetActivityStack->registerActivity(mPairingMenuActivity, false, false, false);
		
	}
	if(!connect(ui->widgetActivityStack, &ActivityStack::currentPageChanged, this, &RemoteWindow::onPageChanged, OC_CONTYPE)){
		qWarning()<<"Could not connect to ActivityStack::currentPageChanged";
	}
	pushPage(mControl->objectName());
}


void RemoteWindow::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		auto r=remote();
		if(!r.isNull()) {
	// [...]
			loadWindowGeometry();
			prepareNavigation();
			prepareActivities();
			ui->widgetNavigation->configure(node());
			updateWindowIcon();
			unboxingWizardDone();
		} else {
			qWarning()<<"WARNING: No Remote in remote window configure";
		}
	}
}


void RemoteWindow::unboxingWizardDone(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mUnboxingWizard->unboxingDone()){
			swapPage(mControl->objectName());
		}
		else{
			mUnboxingWizard->updateStage();
			swapPage(mUnboxingWizard->objectName());
		}
	}
}


void RemoteWindow::prepareNavigation(){
	OC_METHODGATE();
	if(!connect(ui->widgetNavigation, &NodeNavigation::navigateBack, this, &RemoteWindow::navigateBack, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation back button";
	}
	if(!connect(ui->widgetNavigation, &NodeNavigation::openMenu, this, &RemoteWindow::menu, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation menu button";
	}
	ui->widgetNavigation->setState("OctoMY™ Remote", true, true, true);

}



void RemoteWindow::onPageChanged(const StackPage &page){
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->widgetNavigation->setState(page.widget->objectName(), page.headerEnabled, page.backEnabled, page.menuEnabled);
	}
}


void RemoteWindow::applicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT CONFIRM";
	}
	pushPage("TransitionActivity", QStringList() << "quit");
	emit node()->nodeRequestExit(EXIT_SUCCESS);
}


void RemoteWindow::pairing(){
	OC_METHODGATE();
	pushPage("PairingActivity");
}


void RemoteWindow::connection(){
	OC_METHODGATE();
	pushPage("ConnectionActivity");
}


void RemoteWindow::identity(){
	OC_METHODGATE();
	pushPage("IdentityActivity");
}


void RemoteWindow::toggleOnline(bool online){
	OC_METHODGATE();
	Q_UNUSED(online);
	auto r = remote();
	if(!r.isNull()) {
	} else {
		qWarning()<<"WARNING: No Remote";
	}
}


void RemoteWindow::navigateBack(){
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "BACK";
	}
	popPage();
}


void RemoteWindow::menu(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"MENU";
	}
	pushPage(mMainMenu->activityName());
}

void RemoteWindow::requestApplicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT ASK";
	}
	pushPage("MessageActivity", QStringList() << "quit" << "Quit" << "Would you like to quit?" << ":/icons/app/warning.svg" << "true");
}

void RemoteWindow::unboxing(){
	OC_METHODGATE();
	pushPage(mUnboxingWizard->activityName());
}


