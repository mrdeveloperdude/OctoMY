#include "RemoteWindow.hpp"
#include "delivery/ControlBaptismActivity.hpp"
#include "delivery/ControlSipAndSeeActivity.hpp"
#include "remote/AgentSelectActivity.hpp"
#include "ui_RemoteWindow.h"

#include "agent/MessageActivity.hpp"
#include "agent/TransitionActivity.hpp"
#include "delivery/ControlDeliveryActivity.hpp"
#include "pairing/PairingActivity.hpp"
#include "pairing/PairingTrustActivity.hpp"
#include "remote/ControlUnboxingStage.hpp"
#include "remote/ControlUnboxingWizard.hpp"
#include "remote/Remote.hpp"
#include "remote/RemoteController.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>


#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

RemoteWindow::RemoteWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::RemoteWindow)
	, mConfigureHelper("RemoteWindow", true, false, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
	mWaterMark.load(QString(":/images/remote_watermark.svg"));
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
	QSharedPointer<Node> n=node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in remote()";
	}

	return qSharedPointerCast<Remote> (n);
}


void RemoteWindow::prepareActivities(){
	auto r=remote();
	if(!r.isNull()) {
		mMessage = OC_NEW MessageActivity();
		ui->widgetActivityStack->registerPage(mMessage, false, false, false);
		
		mSipAndSeeActivity = OC_NEW ControlSipAndSeeActivity();
		mSipAndSeeActivity->configure(r);
		ui->widgetActivityStack->registerPage(mSipAndSeeActivity, false, false, false);
		
		mBaptismActivity = OC_NEW ControlBaptismActivity();
		mBaptismActivity->configure(r);
		ui->widgetActivityStack->registerPage(mBaptismActivity, false, false, false);
		
		mDeliveryActivity = OC_NEW ControlDeliveryActivity();
		mDeliveryActivity->configure(r);
		ui->widgetActivityStack->registerPage(mDeliveryActivity, false, false, false);
		
		mPairing = OC_NEW PairingActivity();
		mPairing->configure(r);
		ui->widgetActivityStack->registerPage(mPairing, false, false, false);
		
		mPairingTrustActivity = OC_NEW PairingTrustActivity();
		mPairingTrustActivity->configure(r);
		ui->widgetActivityStack->registerPage(mPairingTrustActivity, true, true, false);

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


void RemoteWindow::onPageChanged(const StackPage &page){
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->widgetNavigation->setState(page.widget->objectName(), page.headerEnabled, page.backEnabled, page.menuEnabled);
	}
}


void RemoteWindow::quitApplication(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT CONFIRM";
	}
	pushPage("TransitionActivity", QStringList() << "quit");
	emit node()->nodeRequestExit(EXIT_SUCCESS);
}


void RemoteWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	qDebug()<<"REMOTE-LOG-APPEND: "<<text;
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


void RemoteWindow::onStartQuitApplication(){
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"QUIT ASK";
	}
	pushPage("MessageActivity", QStringList() << "quit" << "Quit" << "Would you like to quit?" << ":/icons/warning.svg" << "true");
}




