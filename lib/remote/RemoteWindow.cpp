#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"

#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QScrollBar>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

RemoteWindow::RemoteWindow(Remote *remote, QWidget *parent) :
	QWidget(parent)
  , ui(new Ui::RemoteWindow)
  , remote(remote)
{
	ui->setupUi(this);
	ui->lineEditHubAddress->configure("","hub-address");
	ui->lineEditHubPort->configure("","hub-port");
	ui->lineEditLocalPort->configure("","listen-port");
	ui->stackedWidget->setCurrentWidget(ui->pageConnect);
	utility::populateComboboxWithLocalAdresses(*ui->comboBoxLocalAddress);
	ui->tryToggleListen->setText("Connect","Connecting...","Connected");
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onTryToggleConnectionChanged(TryToggleState)),WWCONTYPE)){
		qDebug()<<"ERROR: could not connect";
	}
	if(0!=remote){
		ui->labelLocal->setText("WAITING FOR LOCAL");
		ui->labelHub->setText("WAITING FOR HUB");
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
		remote->hookSignals(this);
	}
	else{
		ui->labelLocal->setText("N/A");
		ui->labelHub->setText("N/A");
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
		ui->labelAccelerometer->setText("N/A");
	}
	ui->labelTouch->setText("WAITING FOR TOUCH");
	QPalette p=ui->logScroll->palette();
	p.setColor(QPalette::Base, QColor(0, 0, 0, 64));
	ui->logScroll->setPalette(p);

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

RemoteWindow::~RemoteWindow(){
	delete ui;
}


void RemoteWindow::onTryToggleConnectionChanged(TryToggleState s){
	appendLog("TRYSTATE CHANGED TO "+ToggleStateToSTring(s));
	if(TRYING==s && 0!=remote){
		remote->start(ui->comboBoxLocalAddress->currentText(),ui->lineEditLocalPort->text().toInt(), ui->lineEditHubAddress->text(), ui->lineEditHubPort->text().toInt());
		ui->labelLocal->setText("LOCAL:"+ui->comboBoxLocalAddress->currentText()+":"+ui->lineEditLocalPort->text());
		ui->labelHub->setText("HUB: "+ui->lineEditHubAddress->text()+ ":"+ui->lineEditHubPort->text());
		ui->stackedWidget->setCurrentWidget(ui->pageRunning);
	}
	if(ON==s){
		installEventFilter(this);
	}
	else{
		removeEventFilter(this);
	}
}

void RemoteWindow::onPositionUpdated(const QGeoPositionInfo &info){
	ui->labelGPS->setText("GPS: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
	appendLog("GPS update: "+info.coordinate().toString()+"@"+info.timestamp().toString());
}


void RemoteWindow::onCompassUpdated(QCompassReading *r){
	ui->labelCompass->setText("COMPASS: "+QString::number(r->azimuth()));
}

void RemoteWindow::onAccelerometerUpdated(QAccelerometerReading *r){
	ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void RemoteWindow::onGyroscopeUpdated(QGyroscopeReading *r){
	ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}


void RemoteWindow::keyReleaseEvent(QKeyEvent *e){
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


bool RemoteWindow::eventFilter(QObject *object, QEvent *event){
	qDebug()<<"EVENT: "<<object<<": "<<event;
	if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress ){
		QMouseEvent *r= static_cast<QMouseEvent *>(event);
		ui->labelTouch->setText("TOUCH: <"+QString::number(r->globalX())+", "+QString::number(r->globalY())+">");
	}
	//object == ui->widgetBackground && (
	return false;
}

void RemoteWindow::on_pushButtonConfirmQuit_clicked(){
	exit(0);
}



void RemoteWindow::notifyAndroid(QString s){
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android NOTIF: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/remote/Remote",
											  "notify",
											  "(Ljava/lang/String;)V",
											  QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void RemoteWindow::toastAndroid(QString s){
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android TOAST: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/remote/Remote",
											  "toast",
											  "(Ljava/lang/String;)V",
											  QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void RemoteWindow::on_pushButtonTest_clicked(){
	appendLog("TEST BUTTON CLICKED");
	notifyAndroid("TESTING 123");
	toastAndroid("TOASTING 123");
	remote->sendStatus();
}


void RemoteWindow::appendLog(const QString& text){
	WWMETHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(0!=vsb){
		vsb->setValue(vsb->maximum());
	}
}
