#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "Agent.hpp"
#include "comms/CommsChannel.hpp"
#include "../libutil/utility/Utility.hpp"
#include "hw/actuators/HexyTool.hpp"
#include "hw/BluetoothList.hpp"
#include "basic/Settings.hpp"
#include "security/PortableID.hpp"
#include "audio/OneOffSpeech.hpp"

#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

void AgentWindow::updateIdentity()
{
	if(nullptr!=mAgent) {
		mAgent->updateDiscoveryClient();
		//Set our custom identicon as window icon
		PortableID pid;
		pid.setID(mAgent->keyStore().localKey().id());
		Identicon id(pid);
		QIcon icon;//=windowIcon();
		icon.addPixmap(id.pixmap());
		//	icon.addFile(QStringLiteral(":/icons/agent.svg"), QSize(), QIcon::Normal, QIcon::Off);
		setWindowIcon(icon);
	}
	ui->widgetFace->setAgent(mAgent);
	ui->widgetDelivery->configure(mAgent);
	ui->widgetPairing->configure(mAgent);

}

AgentWindow::AgentWindow(Agent *agent, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentWindow)
	, mAgent(agent)
	, mHexy(nullptr)
	, mCameraAction(new QAction(tr("Camera"), this))
	, mPairingAction(new QAction(tr("Pair"), this))
	, mHardwareAction(new QAction(tr("Configure HW"), this))
	, mPlanAction(new QAction(tr("Plan"), this))
	, mOnlineAction(new QAction(tr("Online"), this))
	, mShowOnlineButtonAction(new QAction(tr("Online Button"), this))
	, mShowFaceAction(new QAction(tr("Show Face"), this))
	, mShowLogAction(new QAction(tr("Show Log"), this))
	, mShowStatsAction(new QAction(tr("Show Stats"), this))
	, mShowBirthCertificateAction(new QAction(tr("Show Birth Certificate"), this))
	, mUnbirthAction(new QAction(tr("Unbirth!"), this))
{
	ui->setupUi(this);

	updateIdentity();

	if(nullptr!=mAgent) {
		//Settings &s=agent->settings();
		//Select correct starting page
		QWidget *startPage=ui->pageRunning;
		ui->widgetDelivery->reset();
		ui->stackedWidget->setCurrentWidget(mAgent->keyStore().fileExists()?startPage:ui->pageDelivery);

		connect(ui->widgetDelivery, &AgentDeliveryWizard::done, [=](bool pairNow) {
			updateIdentity();
			ui->stackedWidget->setCurrentWidget(pairNow?ui->pagePairing:startPage);
		} );

		connect(ui->widgetPairing, &PairingWizard::done, [=]() {
			ui->stackedWidget->setCurrentWidget(startPage);
		} );

		mAgent->hookColorSignals(*ui->widgetFace);

		updateVisibility();
		ui->widgetFace->hookSignals(*this);

		ui->widgetPlanEditor->configure("agent.plan");

		prepareMenu();

		//QString text="Hello, my name is "+mAgent->name()+". I am an octomy agent. What is your bidding master?";
		//QString text="Hello, my name is Bodhi. I am an octomy agent. What is your bidding master? 00 0 01010 010 010 010 010101 ";		PortableID id=mAgent->localNodeAssociate()->toPortableID();		new OneOffSpeech(id, text);
	}

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

AgentWindow::~AgentWindow()
{
	delete ui;
}

void AgentWindow::prepareMenu()
{
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;

	// Camera
	//////////////////


	mCameraAction->setStatusTip(tr("Do the camera dance"));
	mCameraAction->setIcon(QIcon(":/icons/eye.svg"));
	connect(mCameraAction, &QAction::triggered, this, &AgentWindow::onStartCameraPairing);
	mMenu.addAction(mCameraAction);

	// Pairing wizard
	//////////////////

	mPairingAction->setStatusTip(tr("Do the pairing dance"));
	mPairingAction->setIcon(QIcon(":/icons/pair.svg"));
	connect(mPairingAction, &QAction::triggered, this, &AgentWindow::onStartPairing);
	mMenu.addAction(mPairingAction);

	// Hardware wizard
	//////////////////

	mHardwareAction->setStatusTip(tr("Connect to the body of the robot"));
	mHardwareAction->setIcon(QIcon(":/icons/actuator_control.svg"));
	connect(mHardwareAction, &QAction::triggered, this, &AgentWindow::onStartHardware);
	mMenu.addAction(mHardwareAction);

	// Plan editor
	//////////////////
	mPlanAction->setStatusTip(tr("Do the planning dance"));
	mPlanAction->setIcon(QIcon(":/icons/mandate.svg"));
	connect(mPlanAction, &QAction::triggered, this, &AgentWindow::onStartPlanEditor);
	mMenu.addAction(mPlanAction);


	// Online status
	//////////////////
	mOnlineAction->setStatusTip(tr("Toggle availability of Agent"));
	mOnlineAction->setCheckable(true);

	if(nullptr!=s) {
		mOnlineAction->setChecked(s->getCustomSettingBool("octomy.online"));
	}
	connect(mOnlineAction, &QAction::triggered, this, &AgentWindow::onOnlineChanged);
	mMenu.addAction(mOnlineAction);

	// Online button
	//////////////////
	mShowOnlineButtonAction->setStatusTip(tr("Show connect button in main screen"));
	mShowOnlineButtonAction->setCheckable(true);

	if(nullptr!=s) {
		mShowOnlineButtonAction->setChecked(s->getCustomSettingBool("octomy.online.show"));
	}
	connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
	mMenu.addAction(mShowOnlineButtonAction);

	// Show Face
	//////////////////

	mShowFaceAction->setStatusTip(tr("Show face in main screen"));
	mShowFaceAction->setCheckable(true);

	if(nullptr!=s) {
		mShowFaceAction->setChecked(s->getCustomSettingBool("octomy.face"));
	}
	connect(mShowFaceAction, &QAction::triggered, this, &AgentWindow::onFaceVisibilityChanged);
	mMenu.addAction(mShowFaceAction);

	// Show Log scroll
	//////////////////
	mShowLogAction->setStatusTip(tr("Show log in main screen"));
	mShowLogAction->setCheckable(true);
	if(nullptr!=s) {
		mShowLogAction->setChecked(s->getCustomSettingBool("octomy.debug.log"));
	}
	connect(mShowLogAction, &QAction::triggered, this, &AgentWindow::onLogVisibilityChanged);
	mMenu.addAction(mShowLogAction);

	// Show Stats
	//////////////////
	mShowStatsAction->setStatusTip(tr("Show stats in main screen"));
	mShowStatsAction->setCheckable(true);
	if(nullptr!=s) {
		mShowStatsAction->setChecked(s->getCustomSettingBool("octomy.debug.stats"));
	}
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
	connect(mUnbirthAction, &QAction::triggered, this, [=]() {
		if(nullptr!=mAgent) {
			QMessageBox::StandardButton reply = QMessageBox::question(this, "Unbirth", "Are you sure you want to DELETE the personality of this robot forever?", QMessageBox::No|QMessageBox::Yes);
			if (QMessageBox::Yes==reply) {
				mAgent->peers().removeParticipant(mAgent->nodeIdentity()->id());
				mAgent->peers().save();
				mAgent->keyStore().clear();
				updateIdentity();
				ui->widgetDelivery->reset();
				ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
				qDebug()<<"UNBIRTHED!";
			}
		}
	});
	mMenu.addAction(mUnbirthAction);
}



void AgentWindow::updateVisibility()
{
	ui->widgetFace->updateVisibility();
}



void AgentWindow::onConnectionStateChanged(const TryToggleState last, const TryToggleState current)
{
	appendLog("TRYSTATE CHANGED FROM " +ToggleStateToSTring(last) +" TO "+ToggleStateToSTring(current));
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	const bool on=OFF!=current;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.online",on);
	}
	updateVisibility();
	mOnlineAction->setChecked(on);
	switch(current) {
	case(TRYING): {
		mAgent->start(mAgent->nodeIdentity()->localAddress());
		//				ui->labelLocal->setText("LOCAL:"+ui->widgetConnection->getLocalAddress().toString()+":"+QString::number(ui->widgetConnection->getLocalPort()));
		//			ui->labelHub->setText("HUB: "+ui->widgetConnection->getTargetAddress().toString()+ ":"+ QString::number(ui->widgetConnection->getTargetPort()));
	}
	break;
	case(ON): {

	} break;
	default:
	case(OFF): {
		mAgent->stop();
	}
	break;
	}
}

void AgentWindow::onColorChanged(QColor c)
{
//	mAgent->onC
}

void AgentWindow::onOnlineChanged(bool on)
{
	qDebug()<<(on?"ONLINE":"OFFLINE");
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.online",on);
	}
	ui->widgetFace->setConnectionState(on?TryToggleState::TRYING:TryToggleState::OFF );
}


void AgentWindow::onOnlineButtonVisibilityChanged(bool on)
{
	qDebug()<<"ONLINE BUTTON IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.online.show",on);
	}
	updateVisibility();
}

void AgentWindow::onFaceVisibilityChanged(bool on)
{
	qDebug()<<"FACE VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.face",on);
	}
	updateVisibility();
}


void AgentWindow::onLogVisibilityChanged(bool on)
{
	qDebug()<<"LOG VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.debug.log",on);
	}
	updateVisibility();
}

void AgentWindow::onStatsVisibilityChanged(bool on)
{
	qDebug()<<"STATS VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.debug.stats",on);
	}
	updateVisibility();
}


void AgentWindow::onStartShowBirthCertificate()
{
	PortableID pid;
	if(nullptr!=mAgent) {
		QString id=mAgent->keyStore().localKey().id();
		QSharedPointer<NodeAssociate> myAss=mAgent->peers().getParticipant(id);
		if(nullptr!=myAss) {
			pid=myAss->toPortableID();
		} else {
			qWarning()<<"ERROR: No ass";
		}

	}
	ui->widgetBirthCertificate->setPortableID(pid);
	ui->stackedWidget->setCurrentWidget(ui->pageBirthCertificate);
}


void AgentWindow::onStartCameraPairing()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCamera);
}

void AgentWindow::onStartPairing()
{
	ui->widgetPairing->reset();
	ui->stackedWidget->setCurrentWidget(ui->pagePairing);
}


void AgentWindow::onStartHardware()
{
	ui->widgetHardware->reset();
	ui->stackedWidget->setCurrentWidget(ui->pageHardware);
}



void AgentWindow::onStartPlanEditor()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePlan);
}


void AgentWindow::appendLog(const QString& text)
{
	ui->widgetFace->appendLog(text);
}




void AgentWindow::keyReleaseEvent(QKeyEvent *e)
{
	if(Qt::Key_Back==e->key()) {
		/*
		if(ui->pageConnect==ui->stackedWidget->currentWidget()) {
			appendLog("EXITING APP ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConfirmQuit);
		} else if(ui->pageRunning==ui->stackedWidget->currentWidget()) {
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		} else if(ui->pageConfirmQuit==ui->stackedWidget->currentWidget()) {
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		} else {
			appendLog("ERROR ON BACK BUTTON");
		}
		*/
		e->accept();
	} else {
		appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}




void AgentWindow::notifyAndroid(QString s)
{
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android NOTIF: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"notify",
			"(Ljava/lang/String;)V",
			QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void AgentWindow::toastAndroid(QString s)
{
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android TOAST: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"toast",
			"(Ljava/lang/String;)V",
			QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void AgentWindow::on_pushButtonBack_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}


void AgentWindow::on_pushButtonMenu_clicked()
{
	mMenu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}




void AgentWindow::on_pushButtonConfirmQuit_clicked()
{
	exit(0);
}

void AgentWindow::on_pushButtonBack_2_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_4_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_3_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_5_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_6_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}
