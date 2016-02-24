#include "Hub.hpp"

#include <QSharedPointer>
#include <QDataStream>

Hub::Hub(QCommandLineParser &opts, QObject *parent):
	QObject(parent)
  , opts(opts)
  , comms (new CommsChannel(0))
  , lastSentPackets(0)
  , lastReceivedPackets(0)
  , lastLostPackets(0)
  , lastAckedPackets(0)
{
	setObjectName("Hub");
	hookSignals(this);
	if(0!=comms){
		comms->hookSignals(*this);
	}
}

Hub::~Hub(){
	delete comms;
	comms=0;
}


void Hub::hookSignals(QObject *){

}


void Hub::unHookSignals(QObject *){

}

QCommandLineParser &Hub::getOptions(){
	return opts;
}

CommsChannel *Hub::getComms(){
	return comms;
}

void Hub::onReceivePacket(QSharedPointer<QDataStream> ds,QHostAddress,quint16){
	qint32 magic=0;
	*ds>>magic;
	qDebug()<<"GOT MAGIC: "<<magic;
}

void Hub::onError(QString e){
qDebug()<<"error: "<<e;
}



void Hub::onClientAdded(Client *c){
	qDebug()<<"client added:"<<c->getSummary();
}

void Hub::onConnectionStatusChanged(bool s){
	qDebug()<<"conchanged :"<<s;
}
