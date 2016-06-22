#include "CommsChannel.hpp"

#include "basic/Standard.hpp"
#include "hub/HubWindow.hpp"
#include "comms/Client.hpp"
#include "basic/UniquePlatformFingerprint.hpp"
#include "messages/MessageType.hpp"

#include "puppet/Pose.hpp"


#include <QDataStream>
#include <QDateTime>




CommsChannel::CommsChannel(QObject *parent, LogDestination *mw)
	: QObject(parent)
	, udpSocket(this)
	, sendingTimer(this)
	, clients(new ClientDirectory)
	, mw(mw)
	, localSignature(UniquePlatformFingerprint::getInstance().platform().getQuint32(), UniquePlatformFingerprint::getInstance().executable().getQuint32())
	, lastRX(0)
	, lastTX(0)
	, lastRXST(0)
	, lastTXST(0)
	, sendCount(0)
	, recCount(0)
	, connected(false)
{
	setObjectName("CommsChannel");
	if(!connect(&udpSocket, SIGNAL(readyRead()),this, SLOT(onReadyRead()),OC_CONTYPE)){
		qWarning()<<"Could not connect UDP readyRead";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&udpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onUdpError(QAbstractSocket::SocketError)),OC_CONTYPE)){
		qWarning()<<"Could not connect UDP error";
	}
	sendingTimer.setSingleShot(true);
	sendingTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&sendingTimer, SIGNAL(timeout()),this,SLOT(onSendingTimer()),OC_CONTYPE)){
		qWarning()<<"Could not connect sending timer";
	}
	sendingTimer.start(0);
}


ClientDirectory *CommsChannel::getClients(){
	return clients;
}

void CommsChannel::start(QHostAddress bindAddress, quint16 bindPort){
	localSignature.host=bindAddress;
	localSignature.port=bindPort;
	bool b = udpSocket.bind(bindAddress, bindPort);
	qDebug()<<"----- comms bind "<< localSignature.toString()<< (b?" succeeded": " failed");
	if(b){
		sendingTimer.start();
	}
	else{
		stop();
	}
}



void CommsChannel::stop(){
	connected=false;
	sendingTimer.stop();
	udpSocket.close();
	emit connectionStatusChanged(false);
	qDebug()<<"----- comms unbind "<< localSignature.toString();
}

void CommsChannel::setLogOutput(LogDestination *mw){
	this->mw=mw;
}


void CommsChannel::receivePacketRaw( QByteArray datagram, QHostAddress remoteHost , quint16 remotePort ){
	qDebug()<<"RECEIV";
	lastRX=QDateTime::currentMSecsSinceEpoch();
	recCount++;
	if(lastRX-lastRXST>1000){
		qDebug()<<"REC count="<<QString::number(recCount)<<"/sec";
		lastRXST=lastRX;
		recCount=0;
	}
	QSharedPointer<QDataStream> ds(new QDataStream(&datagram, QIODevice::ReadOnly));
	const int header = 20; //magic(4) + version(4) + fingerprint(8) + messageType(4)
	const int size=datagram.size();
	if ( size <= header ){
		emit error("ERROR: Message too short: " +QString::number(datagram.size()));
		return;
	}
	quint32 octomy_protocol_magic=0;
	*ds >> octomy_protocol_magic;
	if(OCTOMY_PROTOCOL_MAGIC!=octomy_protocol_magic){
		emit error("ERROR: OctoMY Protocol Magic mismatch: "+QString::number(octomy_protocol_magic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16));
		return;
	}
	quint32 octomy_protocol_version=0;
	*ds >> octomy_protocol_version;
	switch(octomy_protocol_version){
		case(OCTOMY_PROTOCOL_VERSION_CURRENT):{
				ds->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT);
			}break;
		default:{
				emit error("ERROR: OctoMY Protocol version unsupported: "+QString::number(octomy_protocol_version));
				return;
			}break;
	}
	quint32 remoteClientPlatform=0;
	quint32 remoteClientExecutable=0;
	*ds >> remoteClientPlatform;
	*ds >> remoteClientExecutable;
	ClientSignature remoteSignature(remoteClientPlatform, remoteClientExecutable, remoteHost,remotePort);
	Client *remoteClient=clients->getBest(remoteSignature,true);
	if(0==remoteClient){
		emit error("ERROR: Could not fetch client by id: '"+remoteSignature.toString()+"'");
		return;
	}
	else {
		qDebug()<<"Data received from client '"<<remoteSignature.toString()<<"'";
		unsigned int packet_sequence = 0;
		unsigned int packet_ack = 0;
		unsigned int packet_ack_bits = 0;
		*ds >> packet_sequence;
		*ds >> packet_ack;
		*ds >> packet_ack_bits;
		remoteClient->reliabilitySystem.packetReceived( packet_sequence, size-header );
		remoteClient->reliabilitySystem.processAck( packet_ack, packet_ack_bits );
		remoteClient->receive();
		qint32 octomy_message_type_int=0;
		*ds >> octomy_message_type_int;
		const MessageType octomy_message_type=(MessageType)octomy_message_type_int;
		switch(octomy_message_type){
			case(QUERY):{
				}break;
			case(PING):{
					//TODO: Record PING received time and schedule PONG message to be sent in return
				}break;
			case(PONG):{
					//TODO: Look up matching PING and broadcast positive status
				}break;
			case(QUERY_RESULT):{
				}break;
			case(COMMAND):{
				}break;
			case(STATUS):{
				}break;
			case(NOOP_MSG):{
				}break;
			case(DIRECT_POSE):{
					Pose pose;
					*ds >> pose;
					qDebug()<<"GOT POSE MESSAGE: "<<pose.toString();

				}break;
			default:
			case(INVALID):{
					emit error("ERROR: OctoMY message type invalid: "+QString::number((quint32)octomy_message_type,16));
					return;
				}break;
		}
		//emit receivePacket(ds,host, port);
	}
}


void CommsChannel::onSendingTimer(){
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 timeSinceLastRX=now-lastRX;
	Client *localClient=clients->getBest(localSignature, true);
	if(0==localClient){
		emit error(QStringLiteral("Error fetching local client by id ")+localSignature.toString());
		return;
	}
	if(connected && timeSinceLastRX>CONNECTION_TIMEOUT){
		stop();
	}
	else if(!connected && timeSinceLastRX<=CONNECTION_TIMEOUT){
		connected=true;
		emit connectionStatusChanged(true);
	}
	lastRX=now;
	sendCount++;
	if(now-lastTXST>1000){
		//qDebug()<<"SEND count="<<QString::number(sendCount)<<"/sec";
		lastTXST=now;
		sendCount=0;
	}
	//Prepare a priority list of couriers to process for this packet
	qint64 mostUrgentCourier=1000000;
	QMap<quint64, Courier *> pri;
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
	//Write one message per courier in pri list
	//NOTE: several possible optimizations exist, for example grouping messages
	//to same node int one packet etc.
	if(pri.size()>0){
		for (QMap<quint64, Courier *>::iterator i = pri.begin(), e=pri.end(); i != e; ++i){
			Courier *courier=i.value();
			if(0!=courier){
				QByteArray datagram;
				QDataStream ds(&datagram,QIODevice::WriteOnly);
				// Write a header with protocol "magic number" and a version
				ds << (quint32)OCTOMY_PROTOCOL_MAGIC;//Protocol MAGIC identifier
				ds << (qint32)OCTOMY_PROTOCOL_VERSION_CURRENT;//Protocol version
				ds.setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT); //Qt Datastream version
				//Write client fingerprint
				ds << localSignature.platform;
				ds << localSignature.executable;
				// Write reliability data
				ds<<localClient->reliabilitySystem.localSequence();
				ds<<localClient->reliabilitySystem.remoteSequence();
				ds<<localClient->reliabilitySystem.generateAckBits();
				//Process courier
				qint32 availableBytes=512-(7*4);
				quint32 bytesUsed=courier->sendingOpportunity(ds,availableBytes);
				availableBytes-=bytesUsed;
				const quint32 sz=datagram.size();
				if(sz>512){
					qWarning()<<"ERROR: UDP packet size exceeded 512 bytes, dropping write";
				}
				else if(availableBytes<=0){
					qWarning()<<"ERROR: courier trying to send too much data: "<<QString::number(bytesUsed)<<" , dropping write";
				}
				else{
					const ClientSignature &sig=courier->getDestination();
					const qint64 written=udpSocket.writeDatagram(datagram,sig.host,sig.port);
					qDebug()<<"WROTE "<<written<<" bytes to "<<sig.host<<":"<<sig.port;
					if(written<sz){
						qDebug()<<"ERROR WRITING TO UDP SOCKET:"<<udpSocket.errorString();
						return;
					}
					else{
						//qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
					}
					localClient->send(written);
				}
			}
		}
	}

	//Do idle packet
	else{
		//TODO: make sure idle packet is sent
		//ds<<(qint32)NOOP_MSG;
		//qDebug()<<"IDLE PACKET";
	}

	//Prepare for next round (this implies a stop() )
	sendingTimer.start(MIN(mostUrgentCourier,1000));//Our interval is 1000ms at the longest
}

//Only for testing purposes! Real data should pass through the courier system
//and be dispatched by logic in sending timer
qint64 CommsChannel::sendRawData(QByteArray datagram,ClientSignature sig){
	return udpSocket.writeDatagram(datagram,sig.host,sig.port);
}

QString CommsChannel::getSummary(){
	QString out="TODO";
	/*
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

	QTextStream ts(&out);
	ts<<connected<<" of " << (disconnected+connected) <<" connected\n";
	*/

	return out;
}



//Slot called when data arrives on socket
void CommsChannel::onReadyRead(){
	qDebug()<<"READY READ";
	while (udpSocket.hasPendingDatagrams()){
		qDebug()<<" + UDP PACKET";
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


void CommsChannel::hookSignals(QObject &ob){
	qRegisterMetaType<Client *>("Client *");
	if(!connect(this,SIGNAL(error(QString)),&ob,SLOT(onError(QString)),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(clientAdded(Client *)),&ob,SLOT(onClientAdded(Client *)),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(connectionStatusChanged(bool)),&ob,SLOT(onConnectionStatusChanged(bool)),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
}


void CommsChannel::unHookSignals(QObject &ob){
	qRegisterMetaType<Client *>("Client *");
	if(!disconnect(this,SIGNAL(error(QString)),&ob,SLOT(onError(QString)))){
		qWarning()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(clientAdded(Client *)),&ob,SLOT(onClientAdded(Client *)))){
		qWarning()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(connectionStatusChanged(bool)),&ob,SLOT(onConnectionStatusChanged(bool)))){
		qWarning()<<"could not disconnect "<<ob.objectName();
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
