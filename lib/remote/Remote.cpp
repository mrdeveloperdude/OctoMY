#include "Remote.hpp"

#include "basic/Standard.hpp"
#include "hub/Client.hpp"
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
{
	hookSignals(this);
	if(0!=comms){
		comms->hookSignals(*this);
	}
}

Remote::~Remote(){
	delete comms;
	comms=0;
}

void Remote::start(QString listenAddress, quint16 listenPort, QString hubAddress, quint16 hubPort){
	this->hubAddress=hubAddress;
	this->hubPort=hubPort;
	if(0!=comms){
		qDebug()<<"REMOTE comms start for server "<<listenAddress<<":"<<listenPort<<" (hub "<<hubAddress<<":"<<hubPort<<")";
		comms->start(QHostAddress(listenAddress),listenPort);
		//Get this show started
		sendStatus();
	}
}

void Remote::hookSignals(QObject *o){
	sensors.hookSignals(o);
}


void Remote::unHookSignals(QObject *o){
	sensors.unHookSignals(o);
}

void Remote::onReceivePacket(QSharedPointer<QDataStream> ds,QHostAddress,quint16){
	qint32 magic=0;
	*ds>>magic;
	qDebug()<<"GOT MAGIC: "<<magic;
}

void Remote::onError(QString e){
	qDebug()<<"REMOTE: Comms error: "<<e;
}

void Remote::onClientAdded(Client *c){
	qDebug()<<"REMOTE: Client added: "<<c->getHash();
}

void Remote::onPositionUpdated(const QGeoPositionInfo &info){
	statusMessage.gps=info.coordinate();
	sendStatus();
}


void Remote::onCompassUpdated(QCompassReading *r){
	if(0!=r){
		statusMessage.compassAzimuth=r->azimuth();
		statusMessage.compassAccuracy=r->calibrationLevel();
		sendStatus();
	}
}

void Remote::onAccelerometerUpdated(QAccelerometerReading *r){
	if(0!=r){
		statusMessage.accellerometer=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
	//ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void Remote::onGyroscopeUpdated(QGyroscopeReading *r){
	if(0!=r){
		statusMessage.gyroscope=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
	//ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}


void Remote::sendStatus(){
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	const qint64 interval=now-lastSend;
	if(interval>100){
		QByteArray datagram;
		QDataStream ds(&datagram,QIODevice::WriteOnly);
		ds<<statusMessage;
		//TODO:Convert to use courier instead
//		comms->sendPackage(datagram,QHostAddress(hubAddress),hubPort);
		lastSend=now;
	}
}



















