#include "Remote.hpp"
#include "Standard.hpp"

#include <QDebug>
#include <QDataStream>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>



Remote::Remote(QObject *parent):
	QObject(parent)
  , server (new CommsChannel(0))
  , lastSend(0)
{
	hookSignals(this);
	if(0!=server){
		qRegisterMetaType<QHostAddress>("QHostAddress");
		if(!connect(server, SIGNAL(receivePacket(QByteArray,QHostAddress,quint16)),this,SLOT(onServerReceivePacket(QByteArray,QHostAddress,quint16)),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(server, SIGNAL(error(QString)),this,SLOT(onServerError(QString)),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
		server->start(QHostAddress("127.0.0.1"),54345);
		//Get this show started
		sendStatus();
	}
}

Remote::~Remote(){
	delete server;
	server=0;
}

void Remote::hookSignals(QObject *o){
	sensors.hookSignals(o);
}


void Remote::unHookSignals(QObject *o){
	sensors.unHookSignals(o);
}

void Remote::onServerReceivePacket(QByteArray packet,QHostAddress,quint16){
	QDataStream ds(packet);
	qint32 magic=0;
	ds>>magic;
	qDebug()<<"GOT MAGIC: "<<magic;
}

void Remote::onServerError(QString){

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
		server->sendPackage(datagram,QHostAddress("10.0.0.75"),54345);
		lastSend=now;
	}
}


















