#include "CommsChannel.hpp"

#include "basic/Standard.hpp"
#include "hub/HubWindow.hpp"
#include "comms/Client.hpp"
#include "basic/UniquePlatformFingerprint.hpp"
#include "messages/MessageType.hpp"

#include "puppet/Pose.hpp"


#include <QDataStream>
#include <QDateTime>

quint32 CommsChannel::totalRecCount=0;


CommsChannel::CommsChannel(QObject *parent, LogDestination *mw)
	: QObject(parent)
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
	if(!connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), OC_CONTYPE)){
		qWarning()<<"Could not connect UDP readyRead";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onUdpError(QAbstractSocket::SocketError)), OC_CONTYPE)){
		qWarning()<<"Could not connect UDP error";
	}
	sendingTimer.setSingleShot(true);
	sendingTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&sendingTimer, SIGNAL(timeout()), this, SLOT(onSendingTimer()), OC_CONTYPE)){
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
	lastRX=QDateTime::currentMSecsSinceEpoch();
	totalRecCount++;
	recCount++;
	qDebug()<<totalRecCount<<"--- RECEIV";
	if(lastRX-lastRXST>1000){
		qDebug().noquote()<<"REC count="<<QString::number(recCount)<<"/sec";
		lastRXST=lastRX;
		recCount=0;
	}
	QSharedPointer<QDataStream> ds(new QDataStream(&datagram, QIODevice::ReadOnly));
	const int header = 20; //magic(4) + version(4) + fingerprint(8) + messageType(4)
	const int size=datagram.size();
	int totalAvailable=size;
	qDebug()<<totalRecCount<<"PACKET INITIAL SIZE: "<<size;
	if ( size <= header ){
		QString es=QString::number(totalRecCount)+" ERROR: Message too short: " +QString::number(size);
		qWarning()<<es;
		emit error(es);
		return;
	}
	quint32 octomy_protocol_magic=0;
	*ds >> octomy_protocol_magic;
	totalAvailable-=sizeof(quint32);
	if(OCTOMY_PROTOCOL_MAGIC!=octomy_protocol_magic){
		QString es=QString::number(totalRecCount)+" ERROR: OctoMY Protocol Magic mismatch: "+QString::number(octomy_protocol_magic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16);
		qWarning()<<es;
		emit error(es);
		return;
	}
	quint32 octomy_protocol_version=0;
	*ds >> octomy_protocol_version;
	totalAvailable-=sizeof(quint32);
	switch(octomy_protocol_version){
		case(OCTOMY_PROTOCOL_VERSION_CURRENT):{
				ds->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT);
			}break;
		default:{
				QString es=QString::number(totalRecCount)+"ERROR: OctoMY Protocol version unsupported: "+QString::number(octomy_protocol_version);
				qWarning()<<es;
				emit error(es);
				return;
			}break;
	}
	quint32 remoteClientPlatform=0;
	quint32 remoteClientExecutable=0;
	*ds >> remoteClientPlatform;
	totalAvailable-=sizeof(quint32);
	*ds >> remoteClientExecutable;
	totalAvailable-=sizeof(quint32);
	ClientSignature remoteSignature(remoteClientPlatform, remoteClientExecutable, remoteHost,remotePort);
	Client *remoteClient=clients->getBest(remoteSignature,true);
	if(0==remoteClient){
		QString es=QString::number(totalRecCount)+"ERROR: Could not fetch client by id: '"+remoteSignature.toString()+"'";
		qWarning()<<es;
		emit error(es);
		return;
	}
	else {
		unsigned int packet_sequence = 0;
		unsigned int packet_ack = 0;
		unsigned int packet_ack_bits = 0;
		*ds >> packet_sequence;
		totalAvailable-=sizeof(unsigned int);
		*ds >> packet_ack;
		totalAvailable-=sizeof(unsigned int);
		*ds >> packet_ack_bits;
		totalAvailable-=sizeof(unsigned int);
		qDebug()<<totalRecCount<<"Data received from client '"<<remoteSignature.toString()<<"' with seq="<<packet_sequence<<" ack="<<packet_ack<<" bits="<<packet_ack_bits<<" and bodysize="<<totalAvailable;
		remoteClient->reliabilitySystem.packetReceived( packet_sequence, size-header );
		remoteClient->reliabilitySystem.processAck( packet_ack, packet_ack_bits );
		remoteClient->receive();
		quint16 parts=0;
		const quint32 minAvailableForPart  = ( sizeof(quint32) + sizeof(quint16) );
		while(totalAvailable > minAvailableForPart){
			parts++;
			qDebug()<<totalRecCount<<parts<<"STARTING NEW PART WITH "<<totalAvailable<<" vs. "<<minAvailableForPart;
			quint32 octomy_message_type_int=0;
			*ds >> octomy_message_type_int;
			totalAvailable-=sizeof(quint32);
			quint16 bytesAvailable=0;
			*ds >> bytesAvailable;
			totalAvailable-=sizeof(quint16);
			if(octomy_message_type_int<Courier::FIRST_USER_ID){
				//Use message type enum for built in messages
				const MessageType octomy_message_type=(MessageType)octomy_message_type_int;
				qDebug()<<totalRecCount<<parts<<"MESSAGE TYPE WAS"<<octomy_message_type<<"("<<QString::number(octomy_message_type_int)<<")";
				switch(octomy_message_type){
					case(PING):{
							qDebug()<<totalRecCount<<parts<<"GOT PING";
							//TODO: Record PING received time and schedule PONG message to be sent in return
						}break;
					case(PONG):{
							qDebug()<<totalRecCount<<parts<<"GOT PONG";
							//TODO: Look up matching PING and broadcast positive status
						}break;
					case(NOOP_MSG):{
							qDebug()<<totalRecCount<<parts<<"GOT NOOP_MSG";
						}break;
					case(DIRECT_POSE):{
							Pose pose;
							*ds >> pose;
							totalAvailable-=pose.size();
							qDebug()<<totalRecCount<<parts<<"GOT POSE MESSAGE: "<<pose.toString();

						}break;
					default:
					case(INVALID):{
							QString es=QString::number(totalRecCount)+" "+QString::number(parts)+" ERROR: OctoMY message type invalid: "+QString::number((quint32)octomy_message_type,16);
							qWarning()<<es;
							emit error(es);
							return;
						}break;
				}
			}
			else{
				//Use courier id for extendable messages
				Courier *c=getCourierByID(octomy_message_type_int);
				if(nullptr!=c){
					const quint16 bytesSpent=c->dataReceived(*ds, bytesAvailable);
					const int left=bytesAvailable-bytesSpent;
					qDebug()<<totalRecCount<<parts<<"Courier message "<<octomy_message_type_int<<" "<<c->getName()<<" bavail="<<bytesAvailable<<" tbavail="<<totalAvailable<<" bspent="<<bytesSpent<<" left="<<left<<"  ";
					totalAvailable-=bytesSpent;
					if(left>=0){
						if(left>0){
							qDebug()<<totalRecCount<<parts<<"SKIPPING "<<left<<" LEFTOVER BYTES AFTER COURIER WAS DONE";
							ds->skipRawData(left);
							totalAvailable-=left;
						}
						else{
							qDebug()<<totalRecCount<<parts<<"ALL GOOD. COURIER BEHAVED EXEMPLARY";
						}
					}
					else{
						QString es=QString::number(totalRecCount)+" "+QString::number(parts)+" ERROR: Courier read more than was available!";
						qWarning()<<es;
						emit error(es);
						return;
					}
					qDebug()<<"TAV:"<<totalAvailable<<" AV:"<<bytesAvailable<<" DS.ATEND:"<<ds->atEnd();
				}
				else{
					//TODO: Look at possibility of registering couriers on demand using something like this:
					//emit wakeOnComms(octomy_message_type_int)
					QString es=QString::number(totalRecCount)+" "+QString::number(parts)+" ERROR: No courier found for ID: "+QString::number(octomy_message_type_int);
					qWarning().noquote()<<es;
					emit error(es);
					return;
				}
				qDebug()<<totalRecCount<<parts<<"PART DONE";
			}
		}
		if(totalAvailable > 0 ){
			QString es="WARNING: Not all bytes in datagram were read/used. There were "+QString::number(totalAvailable)+" bytes left after reading "+QString::number(parts)+" parts";
			qWarning()<<es;
			return;
		}
	}
}


void CommsChannel::sendData(const quint64 now, Client *localClient, Courier *courier, const ClientSignature *sig1){
	const ClientSignature *sig=(nullptr!=courier && nullptr==sig1)?(&courier->getDestination()):sig1;
	if(nullptr==sig){
		qWarning()<<"ERROR: no courier and no client when sending data";
		return;
	}
	const qint32 availableBytes=512-(7*4);
	QByteArray datagram;
	QDataStream ds(&datagram,QIODevice::WriteOnly);
	quint32 bytesUsed=0;
	// Write a header with protocol "magic number" and a version
	ds << (quint32)OCTOMY_PROTOCOL_MAGIC;//Protocol MAGIC identifier
	bytesUsed += sizeof(quint32);
	ds << (qint32)OCTOMY_PROTOCOL_VERSION_CURRENT;//Protocol version
	bytesUsed += sizeof(qint32);
	ds.setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT); //Qt Datastream version
	//Write client fingerprint
	ds << localSignature.platform;
	bytesUsed += sizeof(quint32);
	ds << localSignature.executable;
	bytesUsed += sizeof(quint32);
	// Write reliability data
	ds << localClient->reliabilitySystem.localSequence();
	bytesUsed += sizeof(unsigned int);
	ds << localClient->reliabilitySystem.remoteSequence();
	bytesUsed += sizeof(unsigned int);
	ds << localClient->reliabilitySystem.generateAckBits();
	bytesUsed += sizeof(unsigned int);
	qDebug()<<"SEND GLOBAL HEADER SIZE: "<<bytesUsed;
	// Send using courier.
	//NOTE: When courier reports "sendActive" that means that it wants to send,
	//      and so even if it writes 0 bytes, there will be a section id present
	//      in packet reserved for it with the 0 bytes following it
	if(nullptr!=courier){
		ds << courier->getID();
		bytesUsed += sizeof(quint32);
		ds << courier->mandate().payloadSize;
		bytesUsed += sizeof(quint16);
		qDebug()<<"SEND LOCAL HEADER SIZE: "<<bytesUsed<<datagram.size();
		const quint64 opportunityBytes=courier->sendingOpportunity(ds);
		bytesUsed += opportunityBytes;
		qDebug()<<"SEND FULL SIZE: "<<bytesUsed<<datagram.size()<<opportunityBytes;
		courier->setLastOpportunity(now);
	}
	// Send idle packet
	else{
		ds<<(quint32)NOOP_MSG;
		bytesUsed+=sizeof(quint32);
		ds << (quint16)0;
		bytesUsed += sizeof(quint16);
		qDebug()<<"IDLE PACKET";
	}
	const quint32 sz=datagram.size();
	qDebug()<<"SEND DS SIZE: "<<sz;
	if(sz>512){
		qWarning()<<"ERROR: UDP packet size exceeded 512 bytes, dropping write";
	}
	else if( (availableBytes-bytesUsed) <= 0){
		qWarning()<<"ERROR: courier trying to send too much data: "<<QString::number(bytesUsed)<<" , dropping write";
	}
	else{
		const qint64 written=udpSocket.writeDatagram(datagram,sig->host,sig->port);
		qDebug()<<"WROTE "<<written<<" bytes to "<<sig->host<<":"<<sig->port;
		if(written<sz){
			qDebug()<<"ERROR WRITING TO UDP SOCKET:"<<udpSocket.errorString();
			return;
		}
		else{
			//qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		localClient->countSend(written);
	}
}

void CommsChannel::onSendingTimer(){
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 timeSinceLastRX=now-lastRX;
	Client *localClient=clients->getBest(localSignature, true);
	if(nullptr==localClient){
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
	const qint64 MIN_RATE=1000;
	qint64 mostUrgentCourier=MIN_RATE;
	QMap<quint64, Courier *> pri;
	QList <const ClientSignature *> idle;
	for(Courier *c:couriers){
		if(nullptr!=c){
			CourierMandate cm=c->mandate();
			if(cm.sendActive){
				const quint64 last = c->getLastOpportunity();
				const qint64 interval = now - last;
				const qint64 overdue = interval - cm.interval;
				if(cm.interval<mostUrgentCourier){
					mostUrgentCourier=cm.interval;
				}
				//We are overdue
				if(overdue>0) {
					quint64 score=(cm.priority*overdue)/cm.interval;
					pri.insert(score,c);
					qDebug()<<c->getName()<<c->getID()<<"PRICALC: "<<last<<interval<<overdue<<" OVERDUE SCORE:"<<score;
				}
				else{
					const ClientSignature *clisig=&c->getDestination();
					idle.push_back(clisig);
					if (-overdue > mostUrgentCourier){
						mostUrgentCourier=-overdue;
						qDebug()<<c->getName()<<c->getID()<<"PRICALC: "<<last<<interval<<overdue<<" URGENCY:"<<mostUrgentCourier;
					}
					else{
						qDebug()<<c->getName()<<c->getID()<<"PRICALC: "<<last<<interval<<overdue<<" MEH";
					}
				}
			}
		}
	}
	// Write one message per courier in pri list
	// NOTE: several possible optimizations exist, for example grouping messages
	//       to same node in one packet etc.

	for (QMap<quint64, Courier *>::iterator i = pri.begin(), e=pri.end(); i != e; ++i){
		Courier *courier=i.value();
		sendData(now, localClient, courier, nullptr); //Client *localClient, Courier *courier, const ClientSignature *sig1){
	}
	for(const ClientSignature *sig:idle){
		sendData(now, localClient, nullptr, sig);
	}
	// Prepare for next round (this implies a stop() )
	quint64 delay=MIN(mostUrgentCourier,MIN_RATE);
	qDebug()<<"DELAY: "<<delay<<" ("<<mostUrgentCourier<<")";
	sendingTimer.start(delay);
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
	qDebug()<<"----- READY READ";
	while (udpSocket.hasPendingDatagrams()){
		qDebug()<<"      + UDP PACKET";
		QByteArray datagram;
		datagram.resize(udpSocket.pendingDatagramSize());
		QHostAddress host;
		quint16 port=0;
		udpSocket.readDatagram(datagram.data(), datagram.size(), &host, &port);
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
	couriersByID[c.getID()]=&c;
}

void CommsChannel::unregisterCourier(Courier &c){
	if(!couriers.contains(&c)){
		qDebug()<<"ERROR: courier was not registered";
		return;
	}
	couriersByID.remove(c.getID());
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
