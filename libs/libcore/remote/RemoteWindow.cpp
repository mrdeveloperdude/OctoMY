#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "comms/Client.hpp"

#include "basic/Settings.hpp"


#include "../libmap/MapControl.hpp"
#include "../libmap/OSMMapAdapter.hpp"
#include "../libmap/OpenAerialMapAdapter.hpp"
#include "../libmap/MapLayer.hpp"
#include "../libmap/LineString.hpp"


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

	if(0!=remote){

		Settings &s=Settings::getInstance();
		//Select correct starting page
		QWidget *startPage=ui->pageRunning;
		ui->stackedWidgetScreen->setCurrentWidget(s.getCustomSettingBool("octomy.initialized")?startPage:ui->pageWelcome);
		//Make sure to switch page on "done"
		connect(ui->widgetWelcome, &ControlPairingWidget::done, [=]() {
			ui->stackedWidgetScreen->setCurrentWidget(startPage);
		} );

		ui->widgetConnection->configure("remote");
		if(!connect(ui->widgetConnection,SIGNAL(connectStateChanged(TryToggleState)),this,SLOT(onTryToggleConnectionChanged(TryToggleState)),WWCONTYPE)){
			qWarning()<<"ERROR: could not connect";
		}
		if(0!=remote){
			ui->labelLocal->setText("WAITING FOR LOCAL");
			ui->labelHub->setText("WAITING FOR HUB");
			ui->labelGPS->setText("WAITING FOR GPS");
			ui->labelCompass->setText("WAITING FOR COMPASS");
			ui->labelGyroscope->setText("WAITING FOR GYRO");
			ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
			remote->hookSignals(*this);
		}
		else{
			ui->labelLocal->setText("N/A");
			ui->labelHub->setText("N/A");
			ui->labelGPS->setText("N/A");
			ui->labelCompass->setText("N/A");
			ui->labelGyroscope->setText("N/A");
			ui->labelAccelerometer->setText("N/A");
		}
		ui->numericEntryServoTest->configure(500,2500,200,1500," µs","Test remote servo control via network", "servo-position-1");
		if(!connect(ui->numericEntryServoTest,SIGNAL(valueChanged(int)),this,SLOT(onServoPositionChanged(int)))){
			qWarning()<<"ERROR: could not connect";
		}
		ui->labelTouch->setText("WAITING FOR TOUCH");
		QPalette p=ui->logScroll->palette();
		p.setColor(QPalette::Base, QColor(0, 0, 0, 64));
		ui->logScroll->setPalette(p);
		updateControlLevel();
	}
	else{
		setDisabled(true);
	}
#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

RemoteWindow::~RemoteWindow(){
	delete ui;
}

void RemoteWindow::appendLog(const QString& text){
	WWMETHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(0!=vsb){
		vsb->setValue(vsb->maximum());
	}
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



void RemoteWindow::keyReleaseEvent(QKeyEvent *e){
	if(Qt::Key_Back==e->key()){
		if(ui->pageConnect==ui->stackedWidgetScreen->currentWidget()){
			appendLog("EXITING APP ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConfirmQuit);
		}
		else if(ui->pageStatus==ui->stackedWidgetScreen->currentWidget()){
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConnect);
		}
		else if(ui->pageConfirmQuit==ui->stackedWidgetScreen->currentWidget()){
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConnect);
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
	//qDebug()<<"EVENT: "<<object<<": "<<event;
	if ((this==object) && (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress )){
		QMouseEvent *r= static_cast<QMouseEvent *>(event);
		ui->labelTouch->setText("TOUCH: <"+QString::number(r->globalX())+", "+QString::number(r->globalY())+">");
		QPointF p=r->localPos();
		QSize s=size();
		remote->onTouchUpdated(QVector2D(p.x()/(qreal)s.width(), p.y()/(qreal)s.height()));
	}
	//object == ui->widgetBackground && (
	return false;
}

//////////////////////////////////////////////////
//CommsChannel slots

void RemoteWindow::onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16){
	//qDebug()<<"REMOTE comms: "
}

void RemoteWindow::onError(QString e){
	qDebug()<<"REMOTEW comms: error "<<e;
}

void RemoteWindow::onClientAdded(Client *c){

	qDebug()<<"REMOTEW comms: client added "<<(0==c?"null":QString::number(c->getHash(),16));
}

void RemoteWindow::onConnectionStatusChanged(bool c){
	qDebug()<<"REMOTEW comms: new connection status: "<<c;
	ui->widgetConnection->setConnectState(c?ON:OFF);
}

//////////////////////////////////////////////////
//Internal sensor slots

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





void RemoteWindow::onServoPositionChanged(int val){
	if(0!=remote){
		Pose p;
		p.pos1=val;
		remote->onDirectPoseChanged(p);
	}
}


///////////////////////////////////
//Internal UI slots



void RemoteWindow::onTryToggleConnectionChanged(TryToggleState s){
	appendLog("TRYSTATE CHANGED TO "+ToggleStateToSTring(s));
	bool ce=false;
	QWidget *page=ui->pageConnect;
	switch(s){
		case(TRYING):{
				remote->start(ui->widgetConnection->getLocalAddress(),ui->widgetConnection->getLocalPort(), ui->widgetConnection->getTargetAddress(), ui->widgetConnection->getTargetPort());
				ui->labelLocal->setText("LOCAL:"+ui->widgetConnection->getLocalAddress().toString()+":"+QString::number(ui->widgetConnection->getLocalPort()));
				ui->labelHub->setText("HUB: "+ui->widgetConnection->getTargetAddress().toString()+ ":"+ QString::number(ui->widgetConnection->getTargetPort()));
			}break;
		case(ON):{
				page=ui->pageStatus;
				installEventFilter(this);
			}break;
		default:
		case(OFF):{
				removeEventFilter(this);
				ce=true;
			}break;
	}
	ui->widgetConnection->setEnabled(ce);
	ui->stackedWidgetScreen->setCurrentWidget(page);
}


void RemoteWindow::on_pushButtonConfirmQuit_clicked(){
	exit(0);
}


void RemoteWindow::updateControlLevel(){
	qDebug()<<"SWITCHING CONTROL LEVEL TO "<<ui->comboBoxControlLevel->currentText();
	const int idx=ui->comboBoxControlLevel->currentIndex();
	ui->stackedWidgetControl->setCurrentIndex(idx);
	switch(idx){
		default:
		case(0):{

			}break;
		case(1):{

			}break;
		case(2):{

			}break;
		case(3):{

			}break;
		case(4):{

			}break;
	}

}


void RemoteWindow::updateActiveAgent(){
	QString agentName=ui->comboBoxAgent->currentText();
	if("Pair with another"==agentName){
		ui->stackedWidgetScreen->setCurrentWidget(ui->pageWelcome);
	}
	else{
		qDebug()<<"SWITCHING ACTIVE AGENT TO "<<agentName;
		const int idx=ui->comboBoxControlLevel->currentIndex();
		ui->stackedWidgetControl->setCurrentIndex(idx);
	}
}

void RemoteWindow::on_comboBoxControlLevel_currentIndexChanged(int ){
	updateControlLevel();
}

void RemoteWindow::on_pushButtonBack_clicked()
{
	ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
}

void RemoteWindow::on_comboBoxAgent_currentIndexChanged(int index)
{
	updateActiveAgent();
}
