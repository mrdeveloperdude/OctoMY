#include "CommsChannel.hpp"

#include "basic/Standard.hpp"
#include "hub/HubWindow.hpp"
#include "hub/Client.hpp"
#include "basic/UniquePlatformFingerprint.hpp"

#include <QDataStream>
#include <QDateTime>




CommsChannel::CommsChannel(LogDestination *mw):
	QObject(0)
  , udpSocket(this)
  , sendingTimer(this)
  , mw(mw)
  , client_fingerprint(UniquePlatformFingerprint::getInstance().getQuint64())
{
	if(!connect(&udpSocket, SIGNAL(readyRead()),this, SLOT(onReadyRead()),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&udpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onUdpError(QAbstractSocket::SocketError)),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	sendingTimer.setSingleShot(true);
	sendingTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&sendingTimer, SIGNAL(timeout()),this,SLOT(onSendingTimer()),WWCONTYPE)){
		qWarning()<<"Could not connect";
	}
	sendingTimer.start(0);
}


void CommsChannel::start(QHostAddress bindAddress, quint16 bindPort){
	this->bindAddress=bindAddress;
	this->bindPort=bindPort;
	bool b = udpSocket.bind(bindAddress, bindPort);
	sendingTimer.start();
	qDebug()<<"Binding UDP socket for "<< bindAddress <<":"<< bindPort<< (b?" succeeded": " failed");
}

void CommsChannel::stop(){
	sendingTimer.stop();
	udpSocket.close();
	qDebug()<<"Unbinding UDP socket for "<< bindAddress <<":"<< bindPort;
}

void CommsChannel::setLogOutput(LogDestination *mw){
	this->mw=mw;
}

bool CommsChannel::sendPackageRaw( QByteArray ba, QHostAddress host, quint16 port){
	/*
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
			//qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		nl->send(written);


		*/
	return true;
	/*
		Client *nl=getClient(host,port);
		if(0!=nl){

	}
	return false;
	*/
}

#include "messages/MessageType.hpp"

void CommsChannel::receivePacketRaw( QByteArray datagram, QHostAddress host , quint16 port ){
	QSharedPointer<QDataStream> ds(new QDataStream(&datagram, QIODevice::ReadOnly));
	const int header = 20; //magic(4) + version(4) + fingerprint(8) + messageType(4)
	const int size=datagram.size();
	if ( size <= header ){
		emit error("Message too short: " + datagram.size());
		return;
	}
	quint32 octomy_protocol_magic=0;
	*ds >> octomy_protocol_magic;
	if(OCTOMY_PROTOCOL_MAGIC!=octomy_protocol_magic){
		emit error("OctoMY Protocol Magic mismatch: "+QString::number(octomy_protocol_magic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16));
		return;
	}
	quint32 octomy_protocol_version=0;
	*ds >> octomy_protocol_version;
	switch(octomy_protocol_version){
		case(OCTOMY_PROTOCOL_VERSION_CURRENT):{
				ds->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT);
			}break;
		default:{
				emit error("OctoMY Protocol version unsupported: "+QString::number(octomy_protocol_version));
				return;
			}break;
	}
	quint64 client_fingerprint=0;
	*ds >> client_fingerprint;
	Client *client=getClient(client_fingerprint, host,port);
	if(0==client){
		emit error("Error fetching client by id "+QString::number(client_fingerprint,16));
		return;
	}
	else {
		//qDebug()<<"RECEIVED "<<datagram.size()<<" RAW BYTES FROM "<<host<<":"<<port<<" ("<<client_fingerprint<<")";
		unsigned int packet_sequence = 0;
		unsigned int packet_ack = 0;
		unsigned int packet_ack_bits = 0;
		*ds >> packet_sequence;
		*ds >> packet_ack;
		*ds >> packet_ack_bits;
		client->reliabilitySystem.packetReceived( packet_sequence, size-header );
		client->reliabilitySystem.processAck( packet_ack, packet_ack_bits );
		client->receive();
		qint32 octomy_message_type_int=0;
		*ds >> octomy_message_type_int;
		const MessageType octomy_message_type=(MessageType)octomy_message_type_int;
		switch(octomy_message_type){
			case(QUERY):{
				}break;
			case(QUERY_RESULT):{
				}break;
			case(COMMAND):{
				}break;
			case(STATUS):{
				}break;
			case(NOOP_MSG):{
				}break;
			default:
			case(INVALID):{
					emit error("OctoMY message type invalid: "+QString::number((quint32)octomy_message_type,16));
					return;
				}break;
		}
		emit receivePacket(ds,host, port);
	}
}

void CommsChannel::onSendingTimer(){
	Client *client=getClient(client_fingerprint, bindAddress, bindPort);
	if(0==client){
		emit error("Error fetching client by id "+QString::number(client_fingerprint,16));
		return;
	}
	//Prepare a priority list of couriers to process for this packet
	qint64 mostUrgentCourier=1000000;
	QMap<quint64, Courier *> pri;
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	for(QList<Courier *>::iterator i=couriers.begin(),e=couriers.end();i!=e; ++i){
		Courier *c=*i;
		if(0!=c){
			CourierMandate cm=c->mandate();
			if(cm.active){
				qint64 ci=now-cm.lastOportunity;
				qint64 next=ci-cm.interval;
				if(next>0 && next < mostUrgentCourier){
					mostUrgentCourier=next;
				}
				quint64 score=cm.priority*(next>0?next:0);
				pri.insert(score,c);
			}
		}
	}
	QByteArray datagram;
	QDataStream ds(&datagram,QIODevice::WriteOnly);
	// Write a header with protocol "magic number" and a version
	ds << (quint32)OCTOMY_PROTOCOL_MAGIC;//Protocol MAGIC identifier
	ds << (qint32)OCTOMY_PROTOCOL_VERSION_CURRENT;//Protocol version
	ds.setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT); //Qt Datastream version
	//Write client fingerprint
	ds << client_fingerprint;
	// Write CommChannel reliability data
	ds<<client->reliabilitySystem.localSequence();
	ds<<client->reliabilitySystem.remoteSequence();
	ds<<client->reliabilitySystem.generateAckBits();
	//Write message payload as constructed by couriers
	if(pri.size()>0){
		qint32 availableBytes=400;
		for (QMap<quint64, Courier *>::iterator i = pri.begin(), e=pri.end(); i != e; ++i){
			Courier *c=i.value();
			quint32 bytesAdded=c->sendingOportunity(ds,availableBytes);
			availableBytes-=bytesAdded;
			if(availableBytes<=0){
				break;
			}
		}
		const quint32 sz=datagram.size();
		if(sz>512){
			qWarning()<<"WARNING: UDP packet size exceeded 512 bytes, fragmentation may have occured";
		}
		const qint64 written=udpSocket.writeDatagram(datagram,client->host,client->port);
		if(written<sz){
			qDebug()<<"ERROR WRITING TO UDP SOCKET:"<<udpSocket.errorString();
			return;
		}
		else{
			//qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		client->send(written);
	}

	//Do idle packet
	else{
		ds<<(qint32)NOOP_MSG;
		qDebug()<<"IDLE PACKET";
	}

	//Prepare for next round (this implies a stop() )
	sendingTimer.start(MIN(mostUrgentCourier,1000));//Our interval is 1000ms at the longest
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

Client *CommsChannel::getClient(const quint64 fingerprint, const QHostAddress adr, const quint16 port){
	QMap<quint64, Client *> ::const_iterator it=clients.find(fingerprint);
	if(clients.end()==it){
		Client *c=new Client(fingerprint, adr,port, mw);
		it=clients.insert(fingerprint, c);
		emit clientAdded(c);
	}
	return it.value();
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



void CommsChannel::registerCourier(Courier &c){
	if(couriers.contains(&c)){
		qDebug()<<"ERROR: courier was allready registered";
		return;
	}
	couriers.append(&c);
}

void CommsChannel::unregisterCourier(Courier &c){
	if(!couriers.contains(&c)){
		qDebug()<<"ERROR: courier was not registered";
		return;
	}
	couriers.removeAll(&c);
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
