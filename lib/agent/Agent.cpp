#include "Agent.hpp"

#include "basic/Standard.hpp"
#include "hub/Client.hpp"
#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


Agent::Agent(QCommandLineParser &opts, QObject *parent):
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

Agent::~Agent(){
	delete comms;
	comms=0;
}

void Agent::start(QString listenAddress, quint16 listenPort, QString hubAddress, quint16 hubPort){
	this->hubAddress=hubAddress;
	this->hubPort=hubPort;
	if(0!=comms){
		qDebug()<<"Agent comms start for server "<<listenAddress<<":"<<listenPort<<" (hub "<<hubAddress<<":"<<hubPort<<")";
		comms->start(QHostAddress(listenAddress),listenPort);
		//Get this show started
		sendStatus();
	}
}

void Agent::hookSignals(QObject *o){
	sensors.hookSignals(o);
}


void Agent::unHookSignals(QObject *o){
	sensors.unHookSignals(o);
}

void Agent::onReceivePacket(QSharedPointer<QDataStream> ds,QHostAddress,quint16){
	qint32 magic=0;
	*ds>>magic;
	qDebug()<<"GOT MAGIC: "<<magic;
}

void Agent::onError(QString e){
	qDebug()<<"Agent: Comms error: "<<e;
}

void Agent::onClientAdded(Client *c){
	qDebug()<<"Agent: Client added: "<<c->getHash();
}

void Agent::onPositionUpdated(const QGeoPositionInfo &info){
	statusMessage.gps=info.coordinate();
	sendStatus();
}


void Agent::onCompassUpdated(QCompassReading *r){
	if(0!=r){
		statusMessage.compassAzimuth=r->azimuth();
		statusMessage.compassAccuracy=r->calibrationLevel();
		sendStatus();
	}
}

void Agent::onAccelerometerUpdated(QAccelerometerReading *r){
	if(0!=r){
		statusMessage.accellerometer=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
}

void Agent::onGyroscopeUpdated(QGyroscopeReading *r){
	if(0!=r){
		statusMessage.gyroscope=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
}


void Agent::sendStatus(){
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



















