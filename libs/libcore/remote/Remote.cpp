#include "Remote.hpp"

#include "basic/Standard.hpp"
#include "comms/Client.hpp"
#include "puppet/Pose.hpp"


#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>



Remote::Remote(QCommandLineParser &opts, QObject *parent):
	QObject(parent)
  , opts(opts)
  , comms (new CommsChannel(0))
  , lastSend(0)
  , hubPort(0)
  , poseCourier(new DirectPoseCourier(this))
{
	setObjectName("Remote");
	hookSignals(*this);
	if(0!=comms){
		comms->hookSignals(*this);
		comms->registerCourier(*poseCourier);
	}
}

Remote::~Remote(){
	delete comms;
	comms=0;
}

void Remote::start(QHostAddress listenAddress, quint16 listenPort, QHostAddress hubAddress, quint16 hubPort){
	this->hubAddress=hubAddress;
	this->hubPort=hubPort;
	if(0!=comms){
		Client *c=comms->getClients()->getByHost(hubAddress,hubPort,true);
		if(0!=c){
			poseCourier->setDestination(c->signature);
			qDebug()<<"comms.start remote "<<listenAddress<<":"<<listenPort<<" -> hub "<<hubAddress.toString()<<":"<<hubPort<<"";
			comms->start(listenAddress,listenPort);
		}
		else{
			qWarning()<<"ERROR: could not get client for hub";
		}
	}
}

void Remote::hookSignals(QObject &o){
	sensors.hookSignals(o);
	comms->hookSignals(o);
}


void Remote::unHookSignals(QObject &o){
	sensors.unHookSignals(o);
	comms->unHookSignals(o);
}

void Remote::onReceivePacket(QSharedPointer<QDataStream> ds,QHostAddress,quint16){
	qint32 magic=0;
	*ds>>magic;
	qDebug()<<"GOT MAGIC: "<<magic;
}

void Remote::onError(QString e){
	qDebug()<<"Comms error: "<<e;
}

void Remote::onClientAdded(Client *c){
	qDebug()<<"Client added: "<<QString::number(c->getHash(),16);
}


void Remote::onConnectionStatusChanged(bool s){
	qDebug() <<"New connection status: "<<s;
	if(s){

	}
	else{

	}
}





///////////////////////////////////////////

void Remote::onDirectPoseChanged(Pose p){
	if(0!=poseCourier){
		poseCourier->setPose(p);
	}
}

///////////////////////////////////////////




void Remote::onPositionUpdated(const QGeoPositionInfo &info){
	statusMessage.gps=info.coordinate();
}


void Remote::onCompassUpdated(QCompassReading *r){
	if(0!=r){
		statusMessage.compassAzimuth=r->azimuth();
		statusMessage.compassAccuracy=r->calibrationLevel();
	}
}

void Remote::onAccelerometerUpdated(QAccelerometerReading *r){
	if(0!=r){
		statusMessage.accellerometer=QVector3D(r->x(), r->y(), r->z());
	}
	//ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void Remote::onGyroscopeUpdated(QGyroscopeReading *r){
	if(0!=r){
		statusMessage.gyroscope=QVector3D(r->x(), r->y(), r->z());
	}
	//ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}


void Remote::onTouchUpdated(QVector2D v){
	statusMessage.touch=v;
}
