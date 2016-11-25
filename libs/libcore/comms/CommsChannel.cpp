#include "CommsChannel.hpp"
#include "ClientDirectory.hpp"

#include "../libutil/utility/Standard.hpp"
#include "hub/HubWindow.hpp"
#include "comms/Client.hpp"
#include "basic/UniquePlatformFingerprint.hpp"
#include "messages/MessageType.hpp"

#include "puppet/Pose.hpp"


#include <QDataStream>
#include <QDateTime>

quint32 CommsChannel::totalRecCount=0;


CommsChannel::CommsChannel(const QString &id, LogDestination *log, QObject *parent)
	: QObject(parent)
	, mClients(new ClientDirectory)
	, mLog(log)
	, mLocalSignature(id)
	, mLastRX(0)
	, mLastTX(0)
	, mLastRXST(0)
	, mLastTXST(0)
	, mTxCount(0)
	, mRxCount(0)
	, mConnected(false)
{
	setObjectName("CommsChannel");
	if(!connect(&mUDPSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), OC_CONTYPE)) {
		qWarning()<<"Could not connect UDP readyRead";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&mUDPSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onUdpError(QAbstractSocket::SocketError)), OC_CONTYPE)) {
		qWarning()<<"Could not connect UDP error";
	}
	mSendingTimer.setSingleShot(true);
	mSendingTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&mSendingTimer, SIGNAL(timeout()), this, SLOT(onSendingTimer()), OC_CONTYPE)) {
		qWarning()<<"Could not connect sending timer";
	}
}


ClientDirectory *CommsChannel::clients()
{
	return mClients;
}

void CommsChannel::start(NetworkAddress localAddress)
{
	if(isStarted()) {
		stop();
	}
	mLocalSignature.setAddress(localAddress);
	bool b = mUDPSocket.bind(mLocalSignature.address().ip(), mLocalSignature.address().port());
	qDebug()<<"----- comms bind "<< mLocalSignature.toString()<< (b?" succeeded": " failed");
	if(b) {
		mSendingTimer.start();
	} else {
		stop();
	}
}



void CommsChannel::stop()
{
	mConnected=false;
	mSendingTimer.stop();
	mUDPSocket.close();
	emit commsConnectionStatusChanged(false);
	qDebug()<<"----- comms unbind "<< mLocalSignature.toString();
}


ClientSignature CommsChannel::localSignature()
{
	return mLocalSignature;
}

void CommsChannel::setLogOutput(LogDestination *log)
{
	this->mLog=log;
}



void CommsChannel::receivePacketRaw( QByteArray datagram, QHostAddress remoteHost , quint16 remotePort )
{
	mLastRX=QDateTime::currentMSecsSinceEpoch();
	totalRecCount++;
	mRxCount++;
	qDebug()<<totalRecCount<<"--- RECEIV";
	if(mLastRX-mLastRXST>1000) {
		qDebug().noquote()<<"REC count="<<QString::number(mRxCount)<<"/sec";
		mLastRXST=mLastRX;
		mRxCount=0;
	}
	QSharedPointer<QDataStream> ds(new QDataStream(&datagram, QIODevice::ReadOnly));
	const int header = 20; //magic(4) + version(4) + fingerprint(8) + messageType(4)
	const int size=datagram.size();
	int totalAvailable=size;
	qDebug()<<totalRecCount<<"PACKET INITIAL SIZE: "<<size;
	if ( size <= header ) {
		QString es=QString::number(totalRecCount)+" ERROR: Message too short: " +QString::number(size);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	quint32 octomy_protocol_magic=0;
	*ds >> octomy_protocol_magic;
	totalAvailable-=sizeof(quint32);
	if(OCTOMY_PROTOCOL_MAGIC!=octomy_protocol_magic) {
		QString es=QString::number(totalRecCount)+" ERROR: OctoMY Protocol Magic mismatch: "+QString::number(octomy_protocol_magic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	quint32 octomy_protocol_version=0;
	*ds >> octomy_protocol_version;
	totalAvailable-=sizeof(quint32);
	switch(octomy_protocol_version) {
	case(OCTOMY_PROTOCOL_VERSION_CURRENT): {
		ds->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT);
	}
	break;
	default: {
		QString es=QString::number(totalRecCount)+"ERROR: OctoMY Protocol version unsupported: "+QString::number(octomy_protocol_version);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	break;
	}
	quint64 remoteClientShorthandID=0;
	*ds >> remoteClientShorthandID;
	totalAvailable-=sizeof(quint64);
	//TODO: Create shorthand ID from full ID string.
	ClientSignature remoteSignature(remoteClientShorthandID, NetworkAddress(remoteHost,remotePort));
	QSharedPointer<Client> remoteClient=mClients->getBySignature(remoteSignature,true);
	if(0==remoteClient) {
		QString es=QString::number(totalRecCount)+"ERROR: Could not fetch client by id: '"+remoteSignature.toString()+"'";
		qWarning()<<es;
		emit commsError(es);
		return;
	} else {
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
		ReliabilitySystem &rs=remoteClient->reliabilitySystem();
		rs.packetReceived( packet_sequence, size-header );
		rs.processAck( packet_ack, packet_ack_bits );
		remoteClient->receive();
		quint16 parts=0;
		const qint32 minAvailableForPart  = ( sizeof(quint32) + sizeof(quint16) );
		while(totalAvailable > minAvailableForPart) {
			parts++;
			qDebug()<<totalRecCount<<parts<<"STARTING NEW PART WITH "<<totalAvailable<<" vs. "<<minAvailableForPart;
			quint32 octomy_message_type_int=0;
			*ds >> octomy_message_type_int;
			totalAvailable-=sizeof(quint32);
			quint16 bytesAvailable=0;
			*ds >> bytesAvailable;
			totalAvailable-=sizeof(quint16);
			if(octomy_message_type_int<Courier::FIRST_USER_ID) {
				//Use message type enum for built in messages
				const MessageType octomy_message_type=(MessageType)octomy_message_type_int;
				qDebug()<<totalRecCount<<parts<<"MESSAGE TYPE WAS"<<octomy_message_type<<"("<<QString::number(octomy_message_type_int)<<")";
				switch(octomy_message_type) {
				case(PING): {
					qDebug()<<totalRecCount<<parts<<"GOT PING";
					//TODO: Record PING received time and schedule PONG message to be sent in return
				}
				break;
				case(PONG): {
					qDebug()<<totalRecCount<<parts<<"GOT PONG";
					//TODO: Look up matching PING and broadcast positive status
				}
				break;
				case(NOOP_MSG): {
					qDebug()<<totalRecCount<<parts<<"GOT NOOP_MSG";
				}
				break;
				case(DIRECT_POSE): {
					Pose pose;
					*ds >> pose;
					totalAvailable-=pose.size();
					qDebug()<<totalRecCount<<parts<<"GOT POSE MESSAGE: "<<pose.toString();

				}
				break;
				default:
				case(INVALID): {
					QString es=QString::number(totalRecCount)+" "+QString::number(parts)+" ERROR: OctoMY message type invalid: "+QString::number((quint32)octomy_message_type,16);
					qWarning()<<es;
					emit commsError(es);
					return;
				}
				break;
				}
			} else {
				//Use courier id for extendable messages
				Courier *c=getCourierByID(octomy_message_type_int);
				if(nullptr!=c) {
					const quint16 bytesSpent=c->dataReceived(*ds, bytesAvailable);
					const int left=bytesAvailable-bytesSpent;
					qDebug()<<totalRecCount<<parts<<"Courier message "<<octomy_message_type_int<<" "<<c->name()<<" bavail="<<bytesAvailable<<" tbavail="<<totalAvailable<<" bspent="<<bytesSpent<<" left="<<left<<"  ";
					totalAvailable-=bytesSpent;
					if(left>=0) {
						if(left>0) {
							qDebug()<<totalRecCount<<parts<<"SKIPPING "<<left<<" LEFTOVER BYTES AFTER COURIER WAS DONE";
							ds->skipRawData(left);
							totalAvailable-=left;
						} else {
							qDebug()<<totalRecCount<<parts<<"ALL GOOD. COURIER BEHAVED EXEMPLARY";
						}
					} else {
						QString es=QString::number(totalRecCount)+" "+QString::number(parts)+" ERROR: Courier read more than was available!";
						qWarning()<<es;
						emit commsError(es);
						return;
					}
					qDebug()<<"TAV:"<<totalAvailable<<" AV:"<<bytesAvailable<<" DS.ATEND:"<<ds->atEnd();
				} else {
					//TODO: Look at possibility of registering couriers on demand using something like this:
					//emit wakeOnComms(octomy_message_type_int)
					QString es=QString::number(totalRecCount)+" "+QString::number(parts)+" ERROR: No courier found for ID: "+QString::number(octomy_message_type_int);
					qWarning().noquote()<<es;
					emit commsError(es);
					return;
				}
				qDebug()<<totalRecCount<<parts<<"PART DONE";
			}
		}
		if(totalAvailable > 0 ) {
			QString es="WARNING: Not all bytes in datagram were read/used. There were "+QString::number(totalAvailable)+" bytes left after reading "+QString::number(parts)+" parts";
			qWarning()<<es;
			return;
		}
	}
}


void CommsChannel::sendData(const quint64 &now, QSharedPointer<Client> localClient, Courier *courier, const ClientSignature *sig1)
{
	const ClientSignature *sig=(nullptr!=courier && nullptr==sig1)?(&courier->destination()):sig1;
	if(nullptr==sig) {
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
	ds << mLocalSignature.shortHandID();
	bytesUsed += sizeof(quint64);
	// Write reliability data
	ReliabilitySystem &rs=localClient->reliabilitySystem();
	ds << rs.localSequence();
	bytesUsed += sizeof(unsigned int);
	ds << rs.remoteSequence();
	bytesUsed += sizeof(unsigned int);
	ds << rs.generateAckBits();
	bytesUsed += sizeof(unsigned int);
	//qDebug()<<"SEND GLOBAL HEADER SIZE: "<<bytesUsed;
	// Send using courier.
	//NOTE: When courier reports "sendActive" that means that it wants to send,
	//      and so even if it writes 0 bytes, there will be a section id present
	//      in packet reserved for it with the 0 bytes following it
	if(nullptr!=courier) {
		ds << courier->id();
		bytesUsed += sizeof(quint32);
		ds << courier->mandate().payloadSize;
		bytesUsed += sizeof(quint16);
		//qDebug()<<"SEND LOCAL HEADER SIZE: "<<bytesUsed<<datagram.size();
		const quint64 opportunityBytes=courier->sendingOpportunity(ds);
		bytesUsed += opportunityBytes;
		//qDebug()<<"SEND FULL SIZE: "<<bytesUsed<<datagram.size()<<opportunityBytes;
		courier->setLastOpportunity(now);
	}
	// Send idle packet
	else {
		ds<<(quint32)NOOP_MSG;
		bytesUsed+=sizeof(quint32);
		ds << (quint16)0;
		bytesUsed += sizeof(quint16);
		qDebug()<<"IDLE PACKET";
	}
	const quint32 sz=datagram.size();
	//qDebug()<<"SEND DS SIZE: "<<sz;
	if(sz>512) {
		qWarning()<<"ERROR: UDP packet size exceeded 512 bytes, dropping write";
	} else if( (availableBytes-bytesUsed) <= 0) {
		qWarning()<<"ERROR: courier trying to send too much data: "<<QString::number(bytesUsed)<<" , dropping write";
	} else {
		auto na=sig->address();
		const qint64 written=mUDPSocket.writeDatagram(datagram, na.ip(), na.port());
		qDebug()<<"WROTE "<<written<<" bytes to "<<sig;
		if(written<sz) {
			qDebug()<<"ERROR: Only " << written << " of " <<sz<<" written to UDP SOCKET:"<<mUDPSocket.errorString()<< " for destination "<< (sig->toString());
			return;
		} else {
			//qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		localClient->countSend(written);
	}
}

void CommsChannel::onSendingTimer()
{
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 timeSinceLastRX=now-mLastRX;
	QSharedPointer<Client> localClient=mClients->getBySignature(mLocalSignature, true);
	if(nullptr==localClient) {
		emit commsError(QStringLiteral("Error fetching local client by id ")+mLocalSignature.toString());
		return;
	}
	if(mConnected && timeSinceLastRX>CONNECTION_TIMEOUT) {
		stop();
	} else if(!mConnected && timeSinceLastRX<=CONNECTION_TIMEOUT) {
		mConnected=true;
		emit commsConnectionStatusChanged(true);
	}
	mLastRX=now;
	mTxCount++;
	if(now-mLastTXST>1000) {
		//qDebug()<<"SEND count="<<QString::number(sendCount)<<"/sec";
		mLastTXST=now;
		mTxCount=0;
	}
	//Prepare a priority list of couriers to process for this packet
	const qint64 MIN_RATE=1000;
	qint64 mostUrgentCourier=MIN_RATE;
	QMap<quint64, Courier *> pri;
	QList <const ClientSignature *> idle;
	for(Courier *c:mCouriers) {
		if(nullptr!=c) {
			CourierMandate cm=c->mandate();
			if(cm.sendActive) {
				const ClientSignature *clisig=&c->destination();
				if(clisig->isValid()) {
					const quint64 last = c->lastOpportunity();
					const qint64 interval = now - last;
					const qint64 overdue = interval - cm.interval;
					if((qint64)cm.interval<mostUrgentCourier) {
						mostUrgentCourier=cm.interval;
					}
					//We are overdue
					if(overdue>0) {
						quint64 score=(cm.priority*overdue)/cm.interval;
						pri.insert(score,c); //TODO: make this broadcast somehow (use ClientDirectory::getByLastActive() and ClientSignature::isValid() in combination or similar).
						qDebug()<<c->name()<<c->id()<<"PRICALC: "<<last<<interval<<overdue<<" OVERDUE SCORE:"<<score;
					} else {
						idle.push_back(clisig);
						if (-overdue > mostUrgentCourier) {
							mostUrgentCourier=-overdue;
							qDebug()<<c->name()<<c->id()<<"PRICALC: "<<last<<interval<<overdue<<" URGENCY:"<<mostUrgentCourier;
						} else {
							qDebug()<<c->name()<<c->id()<<"PRICALC: "<<last<<interval<<overdue<<" MEH";
						}
					}
				} else {
					qWarning()<<"ERROR: clisig was invalid";
				}

			}
		}
	}
	// Write one message per courier in pri list
	// NOTE: several possible optimizations exist, for example grouping messages
	//       to same node in one packet etc. We may exploit them in the future.

	for (QMap<quint64, Courier *>::iterator i = pri.begin(), e=pri.end(); i != e; ++i) {
		Courier *courier=i.value();
		sendData(now, localClient, courier, nullptr); //Client *localClient, Courier *courier, const ClientSignature *sig1){
		//(const quint64 now, Client *localClient, Courier *courier, const ClientSignature *sig1){
	}
	for(const ClientSignature *sig:idle) {
		sendData(now, localClient, nullptr, sig);
	}
	// Prepare for next round (this implies a stop() )
	quint64 delay=MIN(mostUrgentCourier,MIN_RATE);
	//qDebug()<<"DELAY: "<<delay<<" ("<<mostUrgentCourier<<")";
	mSendingTimer.start(delay);
}

//Only for testing purposes! Real data should pass through the courier system
//and be dispatched by logic in sending timer
qint64 CommsChannel::sendRawData(QByteArray datagram,ClientSignature sig)
{
	return mUDPSocket.writeDatagram(datagram, sig.address().ip(), sig.address().port());
}

QString CommsChannel::getSummary()
{
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
void CommsChannel::onReadyRead()
{
	qDebug()<<"----- READY READ";
	while (mUDPSocket.hasPendingDatagrams()) {
		qDebug()<<"      + UDP PACKET";
		QByteArray datagram;
		datagram.resize(mUDPSocket.pendingDatagramSize());
		QHostAddress host;
		quint16 port=0;
		mUDPSocket.readDatagram(datagram.data(), datagram.size(), &host, &port);
		receivePacketRaw(datagram,host,port);
	}
}


void CommsChannel::onUdpError(QAbstractSocket::SocketError)
{
	emit commsError(mUDPSocket.errorString());
}


void CommsChannel::appendLog(QString msg)
{
	if(0!=mLog) {
		mLog->appendLog(msg);
	}
}


void CommsChannel::setID(const QString &id)
{
	mLocalSignature.setFullID(id);

}

void CommsChannel::hookSignals(QObject &ob)
{
	qRegisterMetaType<Client *>("Client *");
	if(!connect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(commsClientAdded(Client *)),&ob,SLOT(onCommsClientAdded(Client *)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(commsConnectionStatusChanged(bool)),&ob,SLOT(onCommsConnectionStatusChanged(bool)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
}


void CommsChannel::unHookSignals(QObject &ob)
{
	qRegisterMetaType<Client *>("Client *");
	if(!disconnect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(commsClientAdded(Client *)),&ob,SLOT(onCommsClientAdded(Client *)))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(commsConnectionStatusChanged(bool)),&ob,SLOT(onCommsConnectionStatusChanged(bool)))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}
}



void CommsChannel::registerCourier(Courier &c)
{
	if(mCouriers.contains(&c)) {
		qWarning()<<"ERROR: courier was allready registered";
		return;
	} else if(mCouriers.size()>10) {
		qWarning()<<"ERROR: too many couriers, skipping registration of new one: "<<c.id()<<c.name();
		return;
	}
	mCouriers.append(&c);
	mCouriersByID[c.id()]=&c;
	//qDebug()<<"Registered courier: "<<c.id()<<c.name()<<" for a total of "<<mCouriers.size()<<" couriers";
}

void CommsChannel::unregisterCourier(Courier &c)
{
	if(!mCouriers.contains(&c)) {
		qDebug()<<"ERROR: courier was not registered";
		return;
	}
	mCouriersByID.remove(c.id());
	mCouriers.removeAll(&c);
}

int CommsChannel::courierCount()
{
	return mCouriers.count();
}

#ifdef SHOW_ACKS
// show packets that were acked this frame
unsigned int * acks = NULL;
int ack_count = 0;
reliabilitySystem.GetAcks( &acks, ack_count );
if ( ack_count > 0 )
{
	printf( "acks: %d", acks[0] );
	for ( int i = 1; i < ack_count; ++i ) {
		printf( ",%d", acks[i] );
	}
	printf( "\n" );
}
#endif
