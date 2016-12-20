/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

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
	//qDebug()<<totalRecCount<<"--- RECEIV";
	if(mLastRX-mLastRXST>1000) {
		//qDebug().noquote()<<"REC count="<<QString::number(mRxCount)<<"/sec";
		mLastRXST=mLastRX;
		mRxCount=0;
	}
	QSharedPointer<QDataStream> ds(new QDataStream(&datagram, QIODevice::ReadOnly));
	const int header = sizeof(quint32 /*magic*/) +sizeof(quint32 /*version*/) +sizeof(quint64 /*short ID*/)+sizeof(unsigned int /*sequence*/)+sizeof(unsigned int /*ack*/)+sizeof(unsigned int /*ack bits*/);//+sizeof(quint32 /*message type*/);
	const int size=datagram.size();
	int totalAvailable=size;
	//qDebug()<<totalRecCount<<"PACKET INITIAL SIZE: "<<size<<", HEADER CALCULATED SIZE: "<<header<<", THUS RAW BYES EXPECTED: "<<(size-header);
	if ( size <= header ) {
		QString es=QString::number(totalRecCount)+" ERROR: Message too short: " +QString::number(size)+" vs. header: "+QString::number(header);
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
	if(nullptr==remoteClient) {
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
		//qDebug()<<totalRecCount<<"Data received from client '"<<remoteSignature.toString()<<"' with seq="<<packet_sequence<<" ack="<<packet_ack<<" bits="<<packet_ack_bits<<" and bodysize="<<totalAvailable;
		ReliabilitySystem &rs=remoteClient->reliabilitySystem();
		rs.packetReceived( packet_sequence, size-header );
		rs.processAck( packet_ack, packet_ack_bits );
		remoteClient->receive();
		quint16 partsCount=0;
		const qint32 minAvailableForPart  = ( sizeof(quint32)  );
		if(totalAvailable < minAvailableForPart) {
			qWarning()<<"ERROR: NO PARTS DETECTED! Data size too small";
		}
		while(totalAvailable >= minAvailableForPart) {
			partsCount++;
			//qDebug()<<"READING PART #"<<partsCount<<" WITH "<<totalAvailable<<" vs. "<<minAvailableForPart;
			quint32 partMessageTypeID=0;
			*ds >> partMessageTypeID;
			totalAvailable-=sizeof(quint32);
			quint16 partBytesAvailable=0;
			*ds >> partBytesAvailable;
			totalAvailable-=sizeof(quint16);
			if(partMessageTypeID<Courier::FIRST_USER_ID) {
				//Use message type enum for built in messages
				const MessageType partMessageType=(MessageType)partMessageTypeID;
				//qDebug()<<totalRecCount<<"MESSAGE TYPE WAS"<<partMessageType<<"("<<QString::number(partMessageTypeID)<<")";
				switch(partMessageType) {
				case(IDLE): {
					//qDebug()<<totalRecCount<<"GOT IDLE";
					// Idle packet does not contain any data, so we are done here :)
				}
				break;
				default:
				case(INVALID): {
					QString es=QString::number(totalRecCount)+" "+QString::number(partsCount)+" ERROR: OctoMY message type invalid: "+QString::number((quint32)partMessageType,16);
					qWarning()<<es;
					emit commsError(es);
					return;
				}
				break;
				}
			} else {
				//Use courier id for extendable messages
				Courier *c=getCourierByID(partMessageTypeID);
				if(nullptr!=c) {
					const quint16 bytesSpent=c->dataReceived(*ds, partBytesAvailable);
					const int left=partBytesAvailable-bytesSpent;
					//qDebug()<<totalRecCount<<"GOT COURIER MSG "<<partMessageTypeID<<" "<<c->name()<<" bytes avail="<<partBytesAvailable<<" tot avail="<<totalAvailable<<" bytes spent="<<bytesSpent<<" left="<<left<<"  ";
					totalAvailable-=bytesSpent;
					if(left>=0) {
						if(left>0) {
							qWarning()<<totalRecCount<<"WARNING: SKIPPING "<<left<<" LEFTOVER BYTES AFTER COURIER WAS DONE";
							ds->skipRawData(left);
							totalAvailable-=left;
						} else {
							//qDebug()<<totalRecCount<<"ALL GOOD. COURIER BEHAVED EXEMPLARY";
						}
					} else {
						QString es=QString::number(totalRecCount)+" "+QString::number(partsCount)+" ERROR: Courier read more than was available!";
						qWarning()<<es;
						emit commsError(es);
						return;
					}
					//qDebug()<<"tot Available:"<<totalAvailable<<" available:"<<partBytesAvailable<<" dataStream at end:"<<ds->atEnd();
				} else {
					//TODO: Look at possibility of registering couriers on demand using something like this:
					//emit wakeOnComms(octomy_message_type_int)
					QString es=QString::number(totalRecCount)+" "+QString::number(partsCount)+" ERROR: No courier found for ID: "+QString::number(partMessageTypeID);
					qWarning().noquote()<<es;
					emit commsError(es);
					return;
				}
				//qDebug()<<totalRecCount<<partsCount<<"PART DONE";
			}
		}
		if(totalAvailable > 0 ) {
			QString es="WARNING: Not all bytes in datagram were read/used. There were "+QString::number(totalAvailable)+" bytes left after reading "+QString::number(partsCount)+" parts";
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
	const qint32 availableBytes=MAX_UDP_PAYLOAD_SIZE-(7*4);
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
	const quint64 shortID=mLocalSignature.shortHandID();
	ds << shortID;
	bytesUsed += sizeof(shortID);
	// Write reliability data
	// TODO: incorporate writing logic better into reliability class
	// TODO: convert types used in reliability system to Qt for portability piece of mind
	ReliabilitySystem &rs=localClient->reliabilitySystem();
	ds << rs.localSequence();
	bytesUsed += sizeof(unsigned int);
	ds << rs.remoteSequence();
	bytesUsed += sizeof(unsigned int);
	ds << rs.generateAckBits();
	bytesUsed += sizeof(unsigned int);
	//sizeof(quint32 /*magic*/) +sizeof(quint32 /*version*/) +sizeof(quint64 /*short ID*/) +sizeof(unsigned int /*sequence*/)+sizeof(unsigned int /*ack*/)+sizeof(unsigned int /*ack bits*/)+sizeof(quint32 /*message type*/)
	//qDebug()<<"SEND GLOBAL HEADER SIZE: "<<bytesUsed;
	// Send using courier.
	//NOTE: When courier reports "sendActive" that means that it wants to send,
	//      and so even if it writes 0 bytes, there will be a section id present
	//      in packet reserved for it with the 0 bytes following it
	if(nullptr!=courier) {
		const quint32 courierID=courier->id();;
		ds << courierID;
		bytesUsed += sizeof(courierID);
		const quint16 payloadSize=courier->mandate().payloadSize;;
		ds << payloadSize;
		bytesUsed += sizeof(payloadSize);
		//qDebug()<<"SEND LOCAL HEADER SIZE: "<<bytesUsed<<datagram.size();
		const quint64 opportunityBytes=courier->sendingOpportunity(ds);
		bytesUsed += opportunityBytes;
		//qDebug()<<"SEND FULL SIZE: "<<bytesUsed<<datagram.size()<<opportunityBytes;
		courier->setLastOpportunity(now);
	}
	// Send idle packet
	else {
		const quint32 partMessageTypeID=IDLE;
		ds<<partMessageTypeID;
		bytesUsed+=sizeof(partMessageTypeID);
		/* NO DATA NECESSARY
				const quint16 noBytesSpent=(quint16)0;
				ds << noBytesSpent;
				bytesUsed += sizeof(noBytesSpent);
				//qDebug()<<"IDLE PACKET: "<<(nullptr==courier?"NULL":courier->name())<<", "<<(nullptr==localClient?"NULL":localClient->toString());
				*/
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
		//qDebug()<<"WROTE "<<written<<" bytes to "<<sig;
		if(written<sz) {
			qDebug()<<"ERROR: Only " << written << " of " <<sz<<" written to UDP SOCKET:"<<mUDPSocket.errorString()<< " for destination "<< (sig->toString());
			return;
		} else {
			//qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		localClient->countSend(written);
	}
}


void CommsChannel::rescheduleSending(quint64 now)
{
	//qDebug()<<" ... R E S C H E D U L E ... "<<now;
	//Prepare a priority list of couriers to process for this packet
	mMostUrgentCourier=MINIMAL_PACKET_RATE;
	mPri.clear();
	mIdle.clear();
	// Update first
	for(Courier *courier:mCouriers) {
		if(nullptr!=courier) {
			courier->update(now);
		}
	}
	for(Courier *courier:mCouriers) {
		if(nullptr!=courier) {
			CourierMandate cm=courier->mandate();
			if(cm.sendActive) {
				const ClientSignature &clisig=courier->destination();
				if(clisig.isValid()) {
					const quint64 last = courier->lastOpportunity();
					const qint64 interval = now - last;
					const qint64 overdue = interval - cm.interval;
					if((qint64)cm.interval<mMostUrgentCourier) {
						mMostUrgentCourier=cm.interval;
					}
					//We are overdue
					if(overdue>0) {
						quint64 score=(cm.priority*overdue)/cm.interval;
						mPri.insert(score,courier); //TODO: make this broadcast somehow (use ClientDirectory::getByLastActive() and ClientSignature::isValid() in combination or similar).
						//qDebug()<<c->name()<<c->id()<<"PRICALC: "<<last<<interval<<overdue<<" OVERDUE SCORE:"<<score;
					} else {
						mIdle.push_back(&clisig);
						if (-overdue > mMostUrgentCourier) {
							mMostUrgentCourier=-overdue;
							//qDebug()<<c->name()<<c->id()<<"PRICALC: "<<last<<interval<<overdue<<" URGENCY:"<<mostUrgentCourier;
						} else {
							//qDebug()<<c->name()<<c->id()<<"PRICALC: "<<last<<interval<<overdue<<" MEH";
						}
					}
				} else {
					qWarning()<<"ERROR: clisig was invalid: "<<clisig;
				}

			}
		}
	}
	// Prepare for next round (this implies a stop() )
	quint64 delay=qMin(mMostUrgentCourier, (qint64)MINIMAL_PACKET_RATE);
	//qDebug()<<"DELAY: "<<delay<<" ("<<mostUrgentCourier<<")";
	mSendingTimer.start(delay);
}


void CommsChannel::onSendingTimer()
{
	//qDebug()<<"--- SEND";
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
	rescheduleSending(now);
	// Write one message per courier in pri list
	// NOTE: several possible optimizations exist, for example grouping messages
	//       to same node in one packet etc. We may exploit them in the future.
	for (QMap<quint64, Courier *>::iterator i = mPri.begin(), e=mPri.end(); i != e; ++i) {
		Courier *courier=i.value();
		sendData(now, localClient, courier, nullptr); //Client *localClient, Courier *courier, const ClientSignature *sig1){
		//(const quint64 now, Client *localClient, Courier *courier, const ClientSignature *sig1){
	}
	const int isz=mIdle.size();
	if(isz>0) {
		//qDebug()<<"Send "<<isz<<" idle packets";
		for(const ClientSignature *sig:mIdle) {
			sendData(now, localClient, nullptr, sig);
		}
	}
}

//Only for testing purposes! Real data should pass through the courier system
//and be dispatched by logic in sending timer
qint64 CommsChannel::sendRawData(QByteArray datagram,ClientSignature sig)
{
	return mUDPSocket.writeDatagram(datagram, sig.address().ip(), sig.address().port());
}

QString CommsChannel::getSummary()
{
	QString out;

	for(Courier *courier:mCouriers) {
		if(nullptr==courier) {
			out+=" + NULL\n";
		} else {
			out+=" + "+courier->name()+"\n";
		}

	}
	return out;
}


//Slot called when data arrives on socket
void CommsChannel::onReadyRead()
{
	//qDebug()<<"----- READY READ";
	while (mUDPSocket.hasPendingDatagrams()) {
		//qDebug()<<"      + UDP PACKET";
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

void CommsChannel::setHookCommsSignals(QObject &ob, bool hook)
{
	qRegisterMetaType<Client *>("Client *");
	if(hook) {
		if(!connect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(commsClientAdded(Client *)),&ob,SLOT(onCommsClientAdded(Client *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(commsConnectionStatusChanged(bool)),&ob,SLOT(onCommsConnectionStatusChanged(bool)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
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
}




void CommsChannel::setHookCourierSignals(Courier &courier, bool hook)
{
	if(hook) {
		if(!connect(&courier, SIGNAL(reschedule(quint64)),this,SLOT(rescheduleSending(quint64)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect Courier::reschedule for "<<courier.name();
		}

	} else {
		if(!disconnect(&courier, SIGNAL(reschedule(quint64)),this,SLOT(rescheduleSending(quint64)))) {
			qWarning()<<"ERROR: Could not disconnect Courier::reschedule for "<<courier.name();
		}

	}
}



void CommsChannel::setCourierRegistered(Courier &courier, bool reg)
{
	if(reg) {
		if(mCouriers.contains(&courier)) {
			qWarning()<<"ERROR: courier was allready registered";
			return;
		} else if(mCouriers.size()>10) {
			qWarning()<<"ERROR: too many couriers, skipping registration of new one: "<<courier.id()<<courier.name();
			return;
		}
		mCouriers.append(&courier);
		mCouriersByID[courier.id()]=&courier;
		//qDebug()<<"Registered courier: "<<c.id()<<c.name()<<" for a total of "<<mCouriers.size()<<" couriers";
	} else {
		if(!mCouriers.contains(&courier)) {
			qDebug()<<"ERROR: courier was not registered";
			return;
		}
		mCouriersByID.remove(courier.id());
		mCouriers.removeAll(&courier);
	}
	setHookCourierSignals(courier,reg);
}

int CommsChannel::courierCount()
{
	return mCouriers.count();
}



bool CommsChannel::hasCourier(Courier &c) const
{
	return mCouriers.contains(&c);
}

Courier *CommsChannel::getCourierByID(quint32 id) const
{
	if(mCouriersByID.contains(id)) {
		return mCouriersByID[id];
	}
	return nullptr;
}

bool CommsChannel::isStarted() const
{
	return mSendingTimer.isActive();
}
bool CommsChannel::isConnected() const
{
	return mConnected;
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
