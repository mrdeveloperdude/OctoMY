#include "Hub.hpp"

#include <QSharedPointer>
#include <QDataStream>

Hub::Hub(QCommandLineParser &opts, QObject *parent):
	QObject(parent)
  , opts(opts)
  , comms (new CommsChannel(0))
  , zoo (new ZooClient())
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

	//QByteArray OCID=UniquePlatformFingerprint::getInstance().platform().getHEX().toUtf8();

	if(0!=zoo){
		zoo->setURL(QUrl("http://localhost:8123/api"));
		//zoo->setURL(QUrl("http://localhost/lennart/octomy/index.php"));
		//zoo->putNode(OCID); 		zoo->getNode(OCID);
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



ZooClient *Hub::getZoo(){
	return zoo;
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
