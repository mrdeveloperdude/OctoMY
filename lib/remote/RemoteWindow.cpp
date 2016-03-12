#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "comms/Client.hpp"

#include "map/mapcontrol.h"
#include "map/osmmapadapter.h"
#include "map/openaerialmapadapter.h"
#include "map/maplayer.h"
#include "map/linestring.h"


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
		ui->stackedWidgetScreen->setCurrentWidget(ui->pageConnect);
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
		prepareMap();
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


void RemoteWindow::prepareMap(){
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(0!=mc){
		mc->showScale(true);
		QDir dir("./map.cache/");
		if (!dir.exists()) dir.mkpath(".");
		mc->enablePersistentCache ( dir,8192);
		// create MapAdapter to get maps from
		//		qmapcontrol::MapAdapter* mapadapter = new qmapcontrol::OSMMapAdapter();
		//qmapcontrol::MapAdapter* mapadapter = new qmapcontrol::OpenAerialMapAdapter();
		qmapcontrol::TileMapAdapter* mapadapter = new qmapcontrol::TileMapAdapter("tile.openstreetmap.org", "/%1/%2/%3.png", 256, 0, 17);

		//qmapcontrol::TileMapAdapter* mapadapter = new qmapcontrol::TileMapAdapter("cache.kartverket.no/grunnkart/wmts", "/%1/%2/%3.png", 256, 0, 17);

		// create a map layer with the mapadapter
		qmapcontrol::Layer* l = new qmapcontrol::MapLayer("Custom Layer", mapadapter);
		// add Layer to the MapControl
		mc->addLayer(l);

		// create a LineString
		QList<qmapcontrol::Point*> points;

		points.append(new qmapcontrol::Point( 5.456635,60.384571, "HQ"));
		points.append(new qmapcontrol::Point(  5.456249,60.384317, "Launch point"));

		// A QPen also can use transparency
		QPen* linepen = new QPen(QColor(0, 0, 255, 100));
		linepen->setWidth(5);
		// Add the Points and the QPen to a LineString
		qmapcontrol::LineString* ls = new qmapcontrol::LineString(points, "Launch vector", linepen);

		// Add the LineString to the layer
		l->addGeometry(ls);
		homeMap();
	}
}




void RemoteWindow::homeMap() {
	qmapcontrol::MapControl *mc=ui->widgetMap;
	if(0!=mc){
		qDebug()<<"HOME";
		QList<QPointF> londalen;
		londalen <<QPointF(5.452844, 60.385883);
		londalen <<QPointF(5.457945, 60.380353);
		mc->setViewAndZoomIn ( londalen) ;
	}
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



void RemoteWindow::on_pushButtonTest_clicked(){
	appendLog("TEST BUTTON CLICKED");
	notifyAndroid("TESTING 123");
	toastAndroid("TOASTING 123");
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
	}

}

void RemoteWindow::on_comboBoxControlLevel_currentIndexChanged(int ){
	updateControlLevel();
}

void RemoteWindow::on_pushButtonControl_clicked(){
	ui->stackedWidgetScreen->setCurrentWidget(ui->pageControl);
}

void RemoteWindow::on_pushButtonStatus_clicked(){
	ui->stackedWidgetScreen->setCurrentWidget(ui->pageStatus);
}
