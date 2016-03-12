#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "Agent.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "hw/actuators/HexyTool.hpp"

#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
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
	ui->widgetConnection->configure("agent");
	if(!connect(ui->widgetConnection,SIGNAL(connectStateChanged(TryToggleState)),this,SLOT(onTryToggleConnectionChanged(TryToggleState)),WWCONTYPE)){
		qWarning()<<"ERROR: could not connect";
	}
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
	if(0!=agent){
		ui->labelLocal->setText("WAITING FOR LOCAL");
		ui->labelHub->setText("WAITING FOR HUB");
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
		agent->hookSignals(*this);
	}
	else{
		ui->labelLocal->setText("N/A");
		ui->labelHub->setText("N/A");
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
		ui->labelAccelerometer->setText("N/A");
	}
	ui->labelCamera->setText("WAITING FOR CAMERA");
	QPalette p=ui->logScroll->palette();
	p.setColor(QPalette::Base, QColor(0, 0, 0, 64));
	ui->logScroll->setPalette(p);

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

AgentWindow::~AgentWindow(){
	delete ui;
}



void AgentWindow::onTryToggleConnectionChanged(TryToggleState s){
	appendLog("TRYSTATE CHANGED TO "+ToggleStateToSTring(s));
	switch(s){
		case(TRYING):{
				agent->start(ui->widgetConnection->getLocalAddress(),ui->widgetConnection->getLocalPort(), ui->widgetConnection->getTargetAddress(), ui->widgetConnection->getTargetPort());
				ui->labelLocal->setText("LOCAL:"+ui->widgetConnection->getLocalAddress().toString()+":"+QString::number(ui->widgetConnection->getLocalPort()));
				ui->labelHub->setText("HUB: "+ui->widgetConnection->getTargetAddress().toString()+ ":"+ QString::number(ui->widgetConnection->getTargetPort()));
			}break;
		case(ON):{

			}break;
		default:
		case(OFF):{
			}break;
	}
}

void AgentWindow::onPositionUpdated(const QGeoPositionInfo &info){
	ui->labelGPS->setText("GPS: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
	appendLog("GPS update: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
}


void AgentWindow::onCompassUpdated(QCompassReading *r){
	ui->labelCompass->setText("COMPASS: "+QString::number(r->azimuth()));
}

void AgentWindow::onAccelerometerUpdated(QAccelerometerReading *r){
	ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void AgentWindow::onGyroscopeUpdated(QGyroscopeReading *r){
	ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
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


void AgentWindow::on_pushButtonTest_clicked(){
	appendLog("TEST BUTTON CLICKED");
	//notifyAndroid("TESTING 123");
	//toastAndroid("TOASTING 123");
	//agent->sendStatus();
	//ui->stackedWidget->setCurrentIndex(3);
	if(0==hexy){
		hexy=new HexyTool;
		hexy->show();
	}
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
