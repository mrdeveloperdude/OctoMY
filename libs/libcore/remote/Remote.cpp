#include "Remote.hpp"

#include "basic/Standard.hpp"
#include "comms/Client.hpp"
#include "comms/couriers/DirectPoseCourier.hpp"
#include "remote/RemoteWindow.hpp"

#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


Remote::Remote(NodeLauncher<Remote> &launcher, QObject *parent)
	: Node(launcher.getOptions(), "remote", parent)
	, poseCourier(new DirectPoseCourier(this))
	, window(nullptr)
{
}

Remote::~Remote(){
}

void Remote::start(QHostAddress listenAddress, quint16 listenPort, QHostAddress hubAddress, quint16 hubPort){
	if(nullptr!=comms){
		comms->registerCourier(*poseCourier);
	}
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


QWidget *Remote::showWindow(){
	if(nullptr==window){
		window=new RemoteWindow(this, nullptr);
	}
	if(nullptr!=window){
		window->show();
	}
	return window;
}

///////////////////////////////////////////

void Remote::onDirectPoseChanged(Pose p){
	if(0!=poseCourier){
		poseCourier->setPose(p);
	}
}


void Remote::onTouchUpdated(QVector2D v){
	if(nullptr!=sensorMessage){
		sensorMessage->touch=v;
		sendStatus();
	}
}
