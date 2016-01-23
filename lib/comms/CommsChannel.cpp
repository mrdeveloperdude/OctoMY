#include "CommsChannel.hpp"

#include "basic/Standard.hpp"
#include "hub/HubWindow.hpp"
#include "hub/Client.hpp"

#include <QDataStream>
#include <QDateTime>




CommsChannel::CommsChannel(LogDestination *mw):
	QObject(0)
  , udpSocket(this)
  , idleTimer(this)
  , mw(mw)
{
	if(!connect(&udpSocket, SIGNAL(readyRead()),this, SLOT(onReadyRead()),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&udpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onUdpError(QAbstractSocket::SocketError)),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	idleTimer.setInterval(TIMEOUT_TRESHOLD*500);
	idleTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&idleTimer, SIGNAL(timeout()),this,SLOT(onIdleTimer()),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	idleTimer.start();
}


void CommsChannel::start(QHostAddress localAddress, quint16 localPort){
	this->localAddress=localAddress;
	this->localPort=localPort;
	bool b = udpSocket.bind(localAddress, localPort);
	qDebug()<<"Binding UDP socket for "<< localAddress <<":"<< localPort<< (b?" succeeded": " failed");
}

void CommsChannel::stop(){
	udpSocket.close();
	qDebug()<<"Unbinding UDP socket for "<< localAddress <<":"<< localPort;
}

void CommsChannel::setLogOutput(LogDestination *mw){
	this->mw=mw;
}

bool CommsChannel::sendPackage( QByteArray ba, QHostAddress host, quint16 port){
	Client *nl=getClient(host,port);
	if(0!=nl){
		QByteArray datagram;
		QDataStream ds(&datagram,QIODevice::WriteOnly);
		ds<<nl->reliabilitySystem.localSequence();
		ds<<nl->reliabilitySystem.remoteSequence();
		ds<<nl->reliabilitySystem.generateAckBits();
		ds<<ba;
		if(datagram.size()>512){
			qWarning()<<"WARNING: UDP packet size exceeded 512 bytes";
		}
		const qint64 written=udpSocket.writeDatagram(datagram,host,port);
		if(written<0){
			qDebug()<<"ERROR WRITING TO UDP SOCKET:"<<udpSocket.errorString();
			return false;
		}
		else{
			qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		nl->send(written);
		return true;
	}
	return false;
}

void CommsChannel::receivePacketRaw( QByteArray datagram, QHostAddress host , quint16 port ){
	qDebug()<<"RECEIVED "<<datagram.size()<<" RAW BYTES FROM "<<host<<":"<<port;
	Client *nl=getClient(host,port);
	if(0!=nl){
		QSharedPointer<QDataStream> ds(new QDataStream(&datagram, QIODevice::ReadOnly));
		const int header = 12;
		const int size=datagram.size();
		if ( size <= header ){
			qDebug()<<"QD=message too short";
			emit error("Message too short: " + datagram);
			return;
		}
		unsigned int packet_sequence = 0;
		unsigned int packet_ack = 0;
		unsigned int packet_ack_bits = 0;
		*ds >> packet_sequence;
		*ds >> packet_ack;
		*ds >> packet_ack_bits;
		nl->reliabilitySystem.packetReceived( packet_sequence, size- header );
		nl->reliabilitySystem.processAck( packet_ack, packet_ack_bits );
		nl->receive();
		qDebug()<<"QD=recv";
		emit receivePacket(ds,host, port);
	}
	else {
		qDebug()<<"QD=no client for message";
		emit error("Error fetching client on message receive");
	}
}


QString CommsChannel::getSummary(){
	int connected=0;
	int disconnected=0;
	for (QMap<quint64, Client *>::iterator i = clients.begin(), e=clients.end(); i != e; ++i){
		const Client *nl=i.value();
		if(0!=nl){
			if(nl->connected){
				connected++;
			}
			else{
				disconnected++;
			}
		}
	}
	QString out;
	QTextStream ts(&out);
	ts<<connected<<" of " << (disconnected+connected) <<" connected\n";
	return out;
}

QMap<quint64, Client *> &CommsChannel::getClients (){
	return clients;
}

//Slot called when data arrives on socket
void CommsChannel::onReadyRead(){
	while (udpSocket.hasPendingDatagrams()){
		QByteArray datagram;
		datagram.resize(udpSocket.pendingDatagramSize());
		QHostAddress host;
		quint16 port=0;
		udpSocket.readDatagram(datagram.data(), datagram.size(),&host, &port);
		receivePacketRaw(datagram,host,port);
	}
}


void CommsChannel::onUdpError(QAbstractSocket::SocketError){
	emit error(udpSocket.errorString());
}


void CommsChannel::appendLog(QString msg){
	if(0!=mw){
		mw->appendLog(msg);
	}
}

Client *CommsChannel::getClient(QHostAddress host, quint16 port){
	const quint64 hash=Client::generateHash(host,port);
	QMap<quint64, Client *> ::const_iterator it=clients.find(hash);
	if(clients.end()==it){
		Client *c=new Client(host,port, mw);
		it=clients.insert(hash, c);
		emit clientAdded(c);
	}
	return it.value();
}


void CommsChannel::onIdleTimer(){
	QByteArray ba;
	ba.resize(12);
	for (QMap<quint64, Client *>::iterator i = clients.begin(), e=clients.end(); i != e; ++i){
		Client *nl=i.value();
		if(nl->idle()){
			sendPackage(ba, nl->host, nl->port);
		}
	}

#ifdef SHOW_ACKS
	// show packets that were acked this frame
	unsigned int * acks = NULL;
	int ack_count = 0;
	reliabilitySystem.GetAcks( &acks, ack_count );
	if ( ack_count > 0 ){
		printf( "acks: %d", acks[0] );
		for ( int i = 1; i < ack_count; ++i ){
			printf( ",%d", acks[i] );
		}
		printf( "\n" );
	}
#endif

}


void CommsChannel::hookSignals(QObject &ob){
	qRegisterMetaType<Client *>("Client *");
	qRegisterMetaType<QHostAddress>("QHostAddress");
	qRegisterMetaType<QSharedPointer<QDataStream>>("QSharedPointer<QDataStream>");
	if(!connect(this,SIGNAL(receivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16)),&ob,SLOT(onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16)),WWCONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(error(QString)),&ob,SLOT(onError(QString)),WWCONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(clientAdded(Client *)),&ob,SLOT(onClientAdded(Client *)),WWCONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
}


void CommsChannel::unHookSignals(QObject &ob){
	qRegisterMetaType<Client *>("Client *");
	qRegisterMetaType<QHostAddress>("QHostAddress");
	qRegisterMetaType<QSharedPointer<QDataStream>>("QSharedPointer<QDataStream>");
	if(!disconnect(this,SIGNAL(receivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16)),&ob,SLOT(onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16)))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(error(QString)),&ob,SLOT(onError(QString)))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(clientAdded(Client *)),&ob,SLOT(onClientAdded(Client *)))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}

}
