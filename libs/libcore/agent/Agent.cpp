#include "Agent.hpp"

#include "basic/Standard.hpp"
#include "comms/Client.hpp"
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
	setObjectName("Agent");
	hookSignals(*this);
	if(0!=comms){
		comms->hookSignals(*this);
	}
}

Agent::~Agent(){
	delete comms;
	comms=0;
}



void Agent::start(QHostAddress listenAddress, quint16 listenPort, QHostAddress hubAddress, quint16 hubPort){
	this->hubAddress=hubAddress;
	this->hubPort=hubPort;
	if(0!=comms){
		Client *c=comms->getClients()->getByHost(hubAddress,hubPort,true);
		if(0!=c){
			//	poseCourier->setDestination(c->signature);
			qDebug()<<"comms.start remote "<<listenAddress<<":"<<listenPort<<" -> hub "<<hubAddress.toString()<<":"<<hubPort<<"";
			comms->start(listenAddress,listenPort);
		}
		else{
			qWarning()<<"ERROR: could not get client for agent";
		}
	}
}



void Agent::hookSignals(QObject &o){
	sensors.hookSignals(o);
}


void Agent::unHookSignals(QObject &o){
	sensors.unHookSignals(o);
}


void Agent::onConnectionStatusChanged(TryToggleState){

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

void Agent::onConnectionStatusChanged(bool){

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


SensorInput &Agent::getSensorInput(){
	return sensors;
}

CameraList &Agent::getCameras(){
	return cameras;
}
