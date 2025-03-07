#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"


#include "agent/MessageActivity.hpp"
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
	, mMenu(OC_NEW QMenu(tr("Menu"), this))
	, mConfigureHelper("RemoteWindow", true, false, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
	prepareMenu();
	prepareNavigation();
}


RemoteWindow::~RemoteWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui = nullptr;
}


QSharedPointer<Remote> RemoteWindow::remote()
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
		mMessage = OC_NEW MessageActivity();
		ui->widgetActivityStack->registerPage(mMessage, false, false, false);
		
		mTransitionActivity = OC_NEW TransitionActivity();
		ui->widgetActivityStack->registerPage(mTransitionActivity, false, false, false);
		
		mSipAndSeeActivity = OC_NEW ControlSipAndSeeActivity();
		mSipAndSeeActivity->configure(r);
		ui->widgetActivityStack->registerPage(mSipAndSeeActivity, false, false, false);
		
		mBaptismActivity = OC_NEW ControlBaptismActivity();
		mBaptismActivity->configure(r);
		ui->widgetActivityStack->registerPage(mBaptismActivity, false, false, false);
		
		mDeliveryActivity = OC_NEW ControlDeliveryActivity();
		mDeliveryActivity->configure(r);
		ui->widgetActivityStack->registerPage(mDeliveryActivity, false, false, false);
		
		mIdentityActivity = OC_NEW IdentityActivity();
		mIdentityActivity->configure(r);
		ui->widgetActivityStack->registerPage(mIdentityActivity, false, false, false);

		mIdentityMenuActivity = OC_NEW IdentityMenuActivity();
		mIdentityMenuActivity->configure(r);
		ui->widgetActivityStack->registerPage(mIdentityMenuActivity, false, false, false);
		
		mPairingMenuActivity = OC_NEW PairingMenuActivity();
		mPairingMenuActivity->configure(r);
		ui->widgetActivityStack->registerPage(mPairingMenuActivity, false, false, false);
		
		mPairingActivity = OC_NEW PairingActivity();
		mPairingActivity->configure(r);
		ui->widgetActivityStack->registerPage(mPairingActivity, false, false, false);
		
		mPairingTrustActivity = OC_NEW PairingTrustActivity();
		mPairingTrustActivity->configure(r);
		ui->widgetActivityStack->registerPage(mPairingTrustActivity, true, true, false);
		
		mConnectionActivity = OC_NEW ConnectionActivity();
		mConnectionActivity->configure(r);
		ui->widgetActivityStack->registerPage(mConnectionActivity, true, true, true);
		
		mAgentSelectActivity = OC_NEW AgentSelectActivity();
		mAgentSelectActivity->configure(r);
		ui->widgetActivityStack->registerPage(mAgentSelectActivity, true, true, false);
		
		mControl = OC_NEW RemoteController();
		mControl->configure(r);
		ui->widgetActivityStack->registerPage(mControl, true, true, true);
		
		mUnboxingWizard = OC_NEW ControlUnboxingWizard();
		mUnboxingWizard->configure(r);
		ui->widgetActivityStack->registerPage(mUnboxingWizard, true, true, true);
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
	if(!connect(ui->widgetNavigation, &NodeNavigation::openMenu, this, &RemoteWindow::openMenu, OC_CONTYPE)){
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



template <typename MethodType>
QAction* RemoteWindow::addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable) {
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




void RemoteWindow::prepareMenu( )
{
	OC_METHODGATE();
	mQuitAction = addMenuItem(tr("Exit"), ":/icons/no.svg", tr("Terminate execution of this Remote"), &RemoteWindow::requestApplicationShutdown );
	mIdentityAction = addMenuItem(tr("Identity"), ":/icons/identity.svg", tr("Manage the identity of this Remote"), &RemoteWindow::identity );
	mPairingAction = addMenuItem(tr("Pairing"), ":/icons/pair.svg", tr("Manage the pairing of this Remote"), &RemoteWindow::pairing );
	mConnectionAction = addMenuItem(tr("Connection"), ":/icons/network.svg", tr("Manage Remote connection"), &RemoteWindow::connection );
	mToggleOnlineAction = addMenuItem(tr("Toggle Online"), ":/icons/on.svg", tr("Toggle availability of Remote on networks"), &RemoteWindow::toggleOnline, true);
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


void RemoteWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	auto r = remote();
	if(!r.isNull()) {
		r->log(text);
	}
}


QString RemoteWindow::popPage(){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->pop();
	qWarning()<<"POP PAGE " << ret;
	return ret;
}


bool RemoteWindow::pushPage(const QString &title, const QStringList arguments){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->push(title, arguments);
	if(mDebug){
		qDebug()<<"PUSH PAGE " << title << arguments << ret;
	}
	return ret;
}


QString RemoteWindow::swapPage(const QString &title){
	OC_METHODGATE();
	const auto ret = ui->widgetActivityStack->swap(title);
	qWarning()<<"SWAP PAGE " << ret << "-->" << title;
	return ret;
}


void RemoteWindow::navigateBack(){
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "BACK";
	}
	popPage();
}


void RemoteWindow::openMenu(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"MENU";
	}
	mMenu->exec(mapToGlobal(ui->widgetNavigation->menuPos()));
}

void RemoteWindow::requestApplicationShutdown(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT ASK";
	}
	pushPage("MessageActivity", QStringList() << "quit" << "Quit" << "Would you like to quit?" << ":/icons/warning.svg" << "true");
}




