#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "Agent.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "hw/actuators/HexyTool.hpp"
#include "hw/BluetoothList.hpp"
#include "basic/Settings.hpp"
#include "security/PortableID.hpp"

#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

void AgentWindow::updateIdentity(){
	if(nullptr!=agent){
		agent->updateDiscoveryClient();
		//Set our custom identicon as window icon
		PortableID pid;
		pid.setID(agent->getKeyStore().localKey().id());
		Identicon id(pid);
		QIcon icon;//=windowIcon();
		icon.addPixmap(id.pixmap());
		//	icon.addFile(QStringLiteral(":/icons/agent.svg"), QSize(), QIcon::Normal, QIcon::Off);
		setWindowIcon(icon);
	}
	ui->widgetFace->setAgent(agent);
	ui->widgetDelivery->configure(agent);
	ui->widgetPairing->configure(agent);

}

AgentWindow::AgentWindow(Agent *agent, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentWindow)
	, agent(agent)
	, hexy(0)
{
	ui->setupUi(this);

	updateIdentity();

	if(nullptr!=agent){
		Settings &s=agent->getSettings();
		//Select correct starting page
		QWidget *startPage=ui->pageRunning;
		ui->stackedWidget->setCurrentWidget(agent->getKeyStore().hasLocalKeyFile()?startPage:ui->pageDelivery);

		connect(ui->widgetDelivery, &AgentDeliveryWizard::done, [=](bool pairNow) {
			updateIdentity();
			ui->stackedWidget->setCurrentWidget(pairNow?ui->pagePairing:startPage);
		} );

		connect(ui->widgetPairing, &PairingWizard::done, [=]() {
			ui->stackedWidget->setCurrentWidget(startPage);
		} );

		updateVisibility();



		ui->widgetConnection->configure(&s,"agent");
		if(!connect(ui->widgetConnection,SIGNAL(connectStateChanged(TryToggleState)),this,SLOT(onTryToggleConnectionChanged(TryToggleState)),OC_CONTYPE)){
			qWarning()<<"ERROR: could not connect";
		}

		ui->widgetPlanEditor->configure("agent.plan");


		QAction *cameraAction = new QAction(tr("Camera"), this);
		cameraAction->setStatusTip(tr("Do the camera dance"));
		cameraAction->setIcon(QIcon(":/icons/eye.svg"));
		connect(cameraAction, &QAction::triggered, this, &AgentWindow::onStartCameraPairing);
		menu.addAction(cameraAction);


		QAction *pairingAction = new QAction(tr("Pair"), this);
		pairingAction->setStatusTip(tr("Do the pairing dance"));
		pairingAction->setIcon(QIcon(":/icons/pair.svg"));
		connect(pairingAction, &QAction::triggered, this, &AgentWindow::onStartPairing);
		menu.addAction(pairingAction);

		QAction *planAction = new QAction(tr("Plan"), this);
		planAction->setStatusTip(tr("Do the planning dance"));
		planAction->setIcon(QIcon(":/icons/mandate.svg"));
		connect(planAction, &QAction::triggered, this, &AgentWindow::onStartPlanEditor);
		menu.addAction(planAction);


		QAction *faceAction = new QAction(tr("Show Face"), this);
		faceAction->setStatusTip(tr("Show face in main screen"));
		faceAction->setCheckable(true);
		faceAction->setChecked(s.getCustomSettingBool("octomy.face"));
		connect(faceAction, &QAction::triggered, this, &AgentWindow::onFaceVisibilityChanged);
		menu.addAction(faceAction);

		QAction *logAction = new QAction(tr("Show Log"), this);
		logAction->setStatusTip(tr("Show log in main screen"));
		logAction->setCheckable(true);
		logAction->setChecked(s.getCustomSettingBool("octomy.debug.log"));
		connect(logAction, &QAction::triggered, this, &AgentWindow::onLogVisibilityChanged);
		menu.addAction(logAction);


		QAction *statsAction = new QAction(tr("Show Stats"), this);
		statsAction->setStatusTip(tr("Show stats in main screen"));
		statsAction->setCheckable(true);
		statsAction->setChecked(s.getCustomSettingBool("octomy.debug.stats"));
		connect(statsAction, &QAction::triggered, this, &AgentWindow::onStatsVisibilityChanged);
		menu.addAction(statsAction);

		QAction *certAction = new QAction(tr("Show Birth Certificate"), this);
		certAction->setStatusTip(tr("Show the birth certificate of this agent"));
		certAction->setIcon(QIcon(":/icons/certificate.svg"));
		connect(certAction, &QAction::triggered, this, &AgentWindow::onStartShowBirthCertificate);
		menu.addAction(certAction);


		QAction *unbornAction = new QAction(tr("Unbirth!"), this);
		unbornAction->setStatusTip(tr("Delete the identity of this agent to restart birth"));
		unbornAction->setIcon(QIcon(":/icons/kill.svg"));
		Settings *sp=&s;
		connect(unbornAction, &QAction::triggered, this, [=](){
			if(nullptr!=agent){
				QMessageBox::StandardButton reply = QMessageBox::question(this, "Unbirth", "Are you sure you want to DELETE the personality of this robot forever?", QMessageBox::No|QMessageBox::Yes);
				if (QMessageBox::Yes==reply) {
					agent->getKeyStore().clear();
					updateIdentity();
					ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
					qDebug()<<"UNBIRTHED!";
				}
			}
		});
		menu.addAction(unbornAction);
	}

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

AgentWindow::~AgentWindow(){
	delete ui;
}


void AgentWindow::updateVisibility(){
	ui->widgetFace->updateVisibility();
}



void AgentWindow::onTryToggleConnectionChanged(TryToggleState s){
	appendLog("TRYSTATE CHANGED TO "+ToggleStateToSTring(s));
	switch(s){
		case(TRYING):{
				agent->start(ui->widgetConnection->getLocalAddress(),ui->widgetConnection->getLocalPort(), ui->widgetConnection->getTargetAddress(), ui->widgetConnection->getTargetPort());
				//				ui->labelLocal->setText("LOCAL:"+ui->widgetConnection->getLocalAddress().toString()+":"+QString::number(ui->widgetConnection->getLocalPort()));
				//			ui->labelHub->setText("HUB: "+ui->widgetConnection->getTargetAddress().toString()+ ":"+ QString::number(ui->widgetConnection->getTargetPort()));
			}break;
		case(ON):{

			}break;
		default:
		case(OFF):{
			}break;
	}
}

void AgentWindow::onFaceVisibilityChanged(bool on){
	qDebug()<<"FACE VIS IS NOW: "<<on;
	Settings *s=(nullptr!=agent)?(&agent->getSettings()):nullptr;
	if(nullptr!=s){
		s->setCustomSettingBool("octomy.face",on);
	}
	updateVisibility();
}


void AgentWindow::onLogVisibilityChanged(bool on){
	qDebug()<<"LOG VIS IS NOW: "<<on;
	Settings *s=(nullptr!=agent)?(&agent->getSettings()):nullptr;
	if(nullptr!=s){
		s->setCustomSettingBool("octomy.debug.log",on);
	}
	updateVisibility();
}

void AgentWindow::onStatsVisibilityChanged(bool on){
	qDebug()<<"STATS VIS IS NOW: "<<on;
	Settings *s=(nullptr!=agent)?(&agent->getSettings()):nullptr;
	if(nullptr!=s){
		s->setCustomSettingBool("octomy.debug.stats",on);
	}
	updateVisibility();
}


void AgentWindow::onStartShowBirthCertificate(){
	PortableID id;
	id.setID(agent->getKeyStore().localKey().id());
	id.setType(TYPE_AGENT);
	ui->widgetBirthCertificate->setPortableID(id);
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


void AgentWindow::onStartPlanEditor()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePlan);
}


void AgentWindow::appendLog(const QString& text){
	ui->widgetFace->appendLog(text);
}




void AgentWindow::keyReleaseEvent(QKeyEvent *e){
	if(Qt::Key_Back==e->key()){
		if(ui->pageConnect==ui->stackedWidget->currentWidget()){
			appendLog("EXITING APP ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConfirmQuit);
		}
		else if(ui->pageRunning==ui->stackedWidget->currentWidget()){
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		}
		else if(ui->pageConfirmQuit==ui->stackedWidget->currentWidget()){
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		}
		else{
			appendLog("ERROR ON BACK BUTTON");
		}
		e->accept();
	}
	else{
		appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}




void AgentWindow::notifyAndroid(QString s){
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


void AgentWindow::toastAndroid(QString s){
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


void AgentWindow::on_pushButtonBack_clicked(){
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}


void AgentWindow::on_pushButtonMenu_clicked()
{
	menu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}




void AgentWindow::on_pushButtonConfirmQuit_clicked(){
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
