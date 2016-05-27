#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "Agent.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "hw/actuators/HexyTool.hpp"
#include "hw/BluetoothList.hpp"
#include "basic/Settings.hpp"
#include "widgets/WelcomeWidget.hpp"

#include <QDebug>
#include <QScrollBar>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

AgentWindow::AgentWindow(Agent *agent, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentWindow)
	, agent(agent)
	, hexy(0)
{
	ui->setupUi(this);
	Settings &s=Settings::getInstance();
	//Select correct starting page
	QWidget *startPage=ui->pageRunning;
	ui->stackedWidget->setCurrentWidget(s.getCustomSettingBool("octomy.initialized")?startPage:ui->pageWelcome);
	//Make sure to switch page on "done"
	connect(ui->widgetWelcome, &AgentPairingWizard::done, [=]() {

		ui->stackedWidget->setCurrentWidget(startPage);
	} );

	updateVisibility();

	//Set our custom identicon as window icon
	Identicon id(QStringLiteral(":/icons/identicon.svg"),0);
	QIcon icon;//=windowIcon();
	icon.addPixmap(id.pixmap());
	//	icon.addFile(QStringLiteral(":/icons/agent.svg"), QSize(), QIcon::Normal, QIcon::Off);
	setWindowIcon(icon);

	ui->widgetConnection->configure("agent");
	if(!connect(ui->widgetConnection,SIGNAL(connectStateChanged(TryToggleState)),this,SLOT(onTryToggleConnectionChanged(TryToggleState)),WWCONTYPE)){
		qWarning()<<"ERROR: could not connect";
	}


	ui->labelCamera->setText("WAITING FOR CAMERA");
	QPalette p=ui->logScroll->palette();
	p.setColor(QPalette::Base, QColor(0, 0, 0, 64));
	ui->logScroll->setPalette(p);

	ui->widgetPlanEditor->configure("agent.plan");



#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

AgentWindow::~AgentWindow(){
	delete ui;
}


void AgentWindow::updateVisibility(){
	Settings &s=Settings::getInstance();
	ui->widgetFace->setVisible(s.getCustomSettingBool("octomy.face"));
	ui->logScroll->setVisible(s.getCustomSettingBool("octomy.debug"));
	ui->widgetIdenticon->setVisible(s.getCustomSettingBool("octomy.identicon"));
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


void AgentWindow::on_pushButtonConfirmQuit_clicked(){
	exit(0);
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



void AgentWindow::appendLog(const QString& text){
	WWMETHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(0!=vsb){
		vsb->setValue(vsb->maximum());
	}
}



void AgentWindow::on_pushButtonCamera_clicked(){
	ui->stackedWidget->setCurrentWidget(ui->pageCamera);
}

void AgentWindow::on_pushButtonBack_clicked(){
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}


void AgentWindow::on_pushButtonPair_clicked()
{
	ui->widgetWelcome->reset();
	ui->stackedWidget->setCurrentWidget(ui->pageWelcome);

}

void AgentWindow::on_pushButtonPlan_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePlan);
}
