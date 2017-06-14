/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "CommsChannel.hpp"
#include "CommsSessionDirectory.hpp"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "comms/CommsSession.hpp"
#include "messages/MessageType.hpp"

#include "security/KeyStore.hpp"

#include "pose/Pose.hpp"

#include "discovery/NodeAssociateStore.hpp"


#include <QDataStream>
#include <QDateTime>

#define FIRST_STATE_ID ((SESSION_ID_TYPE)MULTIMAGIC_LAST)

const quint64 CommsChannel::CONNECTION_TIMEOUT = MINIMAL_PACKET_RATE + 1000;//1 sec  more than our UDP timeout
// NOTE: We use 512 as the maximum practical UDP size for ipv4 over the internet
//       See this for discussion: http://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
const qint32 CommsChannel::MAX_UDP_PAYLOAD_SIZE = 512;


quint32 CommsChannel::sTotalRecCount = 0;
quint32 CommsChannel::sTotalTxCount = 0;

CommsChannel::CommsChannel(KeyStore &keystore, NodeAssociateStore &peers, QObject *parent)
	: QObject(parent)
	, mKeystore(keystore)
	, mPeers(peers)
	, mSessions(mKeystore)
	, mLocalSessionID(0)
	, mRXRate("CC RX")
	, mTXRate("CC TX")
	, mTXOpportunityRate("CC TX OP")
	, mTXScheduleRate("CC TX SCHED")
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

CommsChannel::CommsChannel(KeyStore &keystore, QObject *parent)
	: QObject(parent)
	, mKeystore(keystore)
	, mPeers(*((NodeAssociateStore *) nullptr ))
	, mSessions(mKeystore)
{
	//TODO: Remove once nobody refers to it any more
	qWarning()<<"ERROR: PLEASE USE THE OTHER CONSTRUCTOR AS THIS IS UNSUPPORTEDF!";
}

CommsSessionDirectory &CommsChannel::sessions()
{
	return mSessions;
}

void CommsChannel::start(NetworkAddress localAddress)
{
	if(isStarted()) {
		stop();
	}
	mLocalAddress=localAddress;
	bool b = mUDPSocket.bind(mLocalAddress.ip(), mLocalAddress.port());
	qDebug()<<"----- comms bind "<< mLocalAddress.toString()<< localID()<< " with interval "<<utility::humanReadableElapsedMS(mSendingTimer.interval()) <<(b?" succeeded": " failed");
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
	qDebug()<<"----- comms unbind "<< mLocalAddress.toString() << localID();
}

bool CommsChannel::isStarted() const
{
	return mSendingTimer.isActive();
}
bool CommsChannel::isConnected() const
{
	return mConnected;
}

////////////////////////////////////////////////////////////////////////////////

struct PacketSendState {
public:
	QSharedPointer<CommsSession> session;
	QByteArray datagram;
	QSharedPointer<QDataStream> stream;
	int bytesUsed;
	QSharedPointer<QDataStream> encStream;
	int encBytesUsed;
	QByteArray octomyProtocolEncryptedMessage;



	static const int OCTOMY_SENDER_ID_SIZE=20;

public:
	PacketSendState()
		: session(nullptr)
		, stream(new QDataStream(&this->datagram, QIODevice::WriteOnly))
		, bytesUsed(0)
		, encStream(new QDataStream(&this->octomyProtocolEncryptedMessage, QIODevice::WriteOnly))
		, encBytesUsed(0)
	{

	}

	void setSession( QSharedPointer<CommsSession> session)
	{
		this->session=session;
	}


	void writeMagic()
	{
		*stream << (quint32)OCTOMY_PROTOCOL_MAGIC;//Protocol MAGIC identifier
		bytesUsed += sizeof(quint32);
	}

	// Write a header with protocol "magic number" and a version
	void writeProtocolVersion()
	{
		*stream << (qint32)OCTOMY_PROTOCOL_VERSION_CURRENT;//Protocol version
		bytesUsed += sizeof(qint32);
		stream->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT); //Qt Datastream version
	}

	void writeSessionID(SESSION_ID_TYPE sessionID)
	{
		*stream << sessionID;
		bytesUsed += sizeof(sessionID);
	}

	void writeCourierID(quint32 courierID)
	{
		*stream << courierID;
		bytesUsed += sizeof(courierID);
	}

	void writePayloadSize(quint16 payloadSize)
	{
		*stream << payloadSize;
		bytesUsed += sizeof(payloadSize);
	}


	void writeReliabilityData(ReliabilitySystem &rs)
	{
#ifdef USE_RELIBABILITY_SYSTEM
		// Write reliability data
		// TODO: incorporate writing logic better into reliability class
		// TODO: convert types used in reliability system to Qt for portability piece of mind
		ReliabilitySystem &rs=localClient->reliabilitySystem();
		*stream << rs.localSequence();
		bytesUsed += sizeof(unsigned int);
		*stream << rs.remoteSequence();
		bytesUsed += sizeof(unsigned int);
		*stream << rs.generateAckBits();
		bytesUsed += sizeof(unsigned int);
#endif
		//sizeof(quint32 /*magic*/) +sizeof(quint32 /*version*/) +sizeof(quint64 /*short ID*/) +sizeof(unsigned int /*sequence*/)+sizeof(unsigned int /*ack*/)+sizeof(unsigned int /*ack bits*/)+sizeof(quint32 /*message type*/)
		//qDebug()<<"SEND GLOBAL HEADER SIZE: "<<bytesUsed;
		// Send using courier.
		//NOTE: When courier reports "sendActive" that means that it wants to send,
		//      and so even if it writes 0 bytes, there will be a section id present
		//      in packet reserved for it with the 0 bytes following it

	}


	/////////////////////////////////////////// ENCRYPTED BEYOND THIS LINE

	static const int OCTOMY_ENCRYPTED_MESSAGE_SIZE=20;

	// Write Pub-key encrypted message body
	void writeProtocolEncryptedMessage()
	{
		auto size=octomyProtocolEncryptedMessage.size();
		*stream << octomyProtocolEncryptedMessage;
		bytesUsed-=size;
	}

	// Write full ID of sender
	void writeEncSenderID(QString senderID)
	{
		const auto raw=QByteArray::fromHex(senderID.toUtf8());
		const auto size=raw.size();
		*encStream << raw;
		encBytesUsed+=size;
	}

	// Write nonce
	void writeEncNonce(quint32 nonce)
	{
		*encStream << nonce;
		encBytesUsed+=sizeof(nonce);
	}

	// Write desired remote session ID
	void writeEncSessionID(SESSION_ID_TYPE sessionID)
	{
		*encStream << sessionID;
		encBytesUsed+=sizeof(sessionID);
	}

};


////////////////////////////////////////////////////////////////////////////////

struct PacketReadState {

public:
	QByteArray datagram;
	QHostAddress remoteHost;
	quint16 remotePort;
	QSharedPointer<QDataStream> stream;

	SESSION_ID_TYPE multimagic;
	const int size;
	int totalAvailable;

	quint32 octomyProtocolMagic;
	//quint8 octomyProtocolFlags;
	quint32 octomyProtocolVersion;

	QByteArray octomyProtocolEncryptedMessage;
	int octomyProtocolEncryptedMessageSize;

	quint32 partMessageTypeID;
	quint16 partBytesAvailable;



	QSharedPointer<QDataStream> encStream;
	int encTotalAvailable;


	QByteArray octomyProtocolSenderIDRaw;
	int octomyProtocolSenderIDRawSize;
	QString octomyProtocolSenderID;

	quint32 octomyProtocolRemoteNonce;
	quint32 octomyProtocolReturnNonce;

	SESSION_ID_TYPE octomyProtocolDesiredRemoteSessionID;

	static const int OCTOMY_SENDER_ID_SIZE=20;

public:
	PacketReadState(QByteArray datagram, QHostAddress remoteHost , quint16 remotePort)
		: datagram(datagram)
		, remoteHost(remoteHost)
		, remotePort(remotePort)
		, stream(new QDataStream(&this->datagram, QIODevice::ReadOnly))
		, multimagic(0)
		, size(datagram.size())
		, totalAvailable(size)
		, octomyProtocolMagic(0)
		  //, octomyProtocolFlags(0)
		, octomyProtocolVersion(0)
		, octomyProtocolEncryptedMessageSize(0)
		, partMessageTypeID(0)
		, partBytesAvailable(0)
		, encStream()
		, encTotalAvailable(0)
		, octomyProtocolSenderIDRawSize(0)
		, octomyProtocolRemoteNonce(0)
		, octomyProtocolReturnNonce(0)
		, octomyProtocolDesiredRemoteSessionID(0)
	{

	}


	// Read multimagic
	void readMultimagic()
	{
		*stream >> multimagic;
		totalAvailable-=sizeof(multimagic);
	}

	// Read protocol MAGIC
	void readProtocolMagic()
	{
		*stream >> octomyProtocolMagic;
		totalAvailable-=sizeof(octomyProtocolMagic);
	}

	/*
	// Read protocol FLAGS
	void readProtocolFlags()
	{
		*stream >> octomyProtocolFlags;
		totalAvailable-=sizeof(octomyProtocolFlags);
	}
	*/

	// Read protocol VERSION
	void readProtocolVersion()
	{
		*stream >> octomyProtocolVersion;
		totalAvailable-=sizeof(octomyProtocolVersion);
	}



	void readPartMessageTypeID()
	{
		partMessageTypeID=0;
		*stream >> partMessageTypeID;
		totalAvailable-=sizeof(partMessageTypeID);
	}

	void readPartBytesAvailable()
	{
		partBytesAvailable=0;
		*stream >> partBytesAvailable;
		totalAvailable-=sizeof(partBytesAvailable);
	}

	/////////////////////////////////////////// ENCRYPTED BEYOND THIS LINE

	static const int OCTOMY_ENCRYPTED_MESSAGE_SIZE=20;

	// Read Pub-key encrypted message body
	void readProtocolEncryptedMessage()
	{
		*stream >> octomyProtocolEncryptedMessage;
		octomyProtocolEncryptedMessageSize=octomyProtocolEncryptedMessage.size();
		totalAvailable-=octomyProtocolEncryptedMessageSize;

		if(octomyProtocolEncryptedMessageSize>0) {
			encStream=QSharedPointer<QDataStream> (new QDataStream(&this->octomyProtocolEncryptedMessage, QIODevice::ReadOnly));
			encTotalAvailable=octomyProtocolEncryptedMessageSize;
		}

	}


	// Extract full ID of sender
	void readEncSenderID()
	{
		*encStream >> octomyProtocolSenderIDRaw;
		octomyProtocolSenderIDRawSize=octomyProtocolSenderIDRaw.size();
		encTotalAvailable-=octomyProtocolSenderIDRawSize;
		octomyProtocolSenderID=octomyProtocolSenderIDRaw.toHex();
	}


	// Extract remote nonce
	void readEncRemoteNonce()
	{
		octomyProtocolRemoteNonce=0;
		*encStream >> octomyProtocolRemoteNonce;
		encTotalAvailable-=sizeof(octomyProtocolRemoteNonce);
	}


	// Extract return nonce
	void readEncReturnNonce()
	{
		octomyProtocolReturnNonce=0;
		*encStream >> octomyProtocolReturnNonce;
		encTotalAvailable-=sizeof(octomyProtocolReturnNonce);
	}


	// Extract desired remote session ID
	void readEncDesiredRemoteSessionID()
	{
		octomyProtocolDesiredRemoteSessionID=0;
		*encStream >> octomyProtocolDesiredRemoteSessionID;
		encTotalAvailable-=sizeof(octomyProtocolDesiredRemoteSessionID);
	}

};



bool CommsChannel::recieveEncryptedBody(PacketReadState &state)
{
	// Read Pub-key encrypted message body
	state.readProtocolEncryptedMessage();
	if(state.octomyProtocolEncryptedMessageSize != state.OCTOMY_ENCRYPTED_MESSAGE_SIZE) { // Encrypted message should be OCTOMY_ENCRYPTED_MESSAGE_SIZE bytes, no more, no less
		QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less encrypted message size mismatch: "+QString::number(state.octomyProtocolEncryptedMessageSize)+ " vs. "+QString::number(state.OCTOMY_ENCRYPTED_MESSAGE_SIZE);
		qWarning()<<es;
		emit commsError(es);
		return false;
	}

	// Decrypt message body using local private-key
	Key &localKey=mKeystore.localKey();
	QByteArray octomyProtocolDecryptedMessage=localKey.decrypt(state.octomyProtocolEncryptedMessage);
	const int octomyProtocolDecryptedMessageSize=octomyProtocolDecryptedMessage.size();
	if(0 == octomyProtocolDecryptedMessageSize) { // Size of decrypted message should be non-zero
		QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less nonce decryption failed";
		qWarning()<<es;
		emit commsError(es);
		return false;
	}
	return true;
}

bool CommsChannel::recieveMagicAndVersion(PacketReadState &state)
{
	state.readProtocolMagic();
	if(OCTOMY_PROTOCOL_MAGIC!=state.octomyProtocolMagic) {
		QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Magic mismatch: "+QString::number(state.octomyProtocolMagic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16);
		qWarning()<<es;
		emit commsError(es); //TODO: Handle this by making a few retries before requiering user to aprove further retries (to avoid endless hammering)
		return false;
	}

	state.readProtocolVersion();
	switch(state.octomyProtocolVersion) {
	case(OCTOMY_PROTOCOL_VERSION_CURRENT): {
		state.stream->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT);
	}
	break;
	// TODO: Look at good ways to stay backward compatible (long term goal, right now this is not feasible due to the frequent changes to the protocol)
	default: {
		QString es=QString::number(sTotalRecCount)+"ERROR: OctoMY Protocol version unsupported: "+QString::number(state.octomyProtocolVersion);
		qWarning()<<es;
		emit commsError(es);
		return false;
	}
	break;
	}
	return true;
}



void CommsChannel::recieveIdle(PacketReadState &state)
{
	qDebug()<<"RECEIVED IDLE PACKET";
}

// 1. A sends SYN to B: Hi B. Here is my ( FULL-ID + NONCE + A's DESIRED SESSION-ID ) ENCODED WITH YOUR PUBKEY.
void CommsChannel::recieveSyn(PacketReadState &state)
{
	qDebug()<<"RECEIVED SYN PACKET";


	// Did handshake complete already?
	//if(session->established()) {
	// TODO: Handle this case:
	/*
					When an initial packet is received after session was already established the following logic applies:
						+ If packet timestamp was before session-established-and-confirmed timestamp, it is ignored
						+ The packet is logged and the stream is examined for packets indicating the original session is still in effect.
						+ If there were no sign of the initial session after a timeout of X seconds, the session is torn down, and the last of the session initial packet is answered to start a new hand shake
						+ If one or more packets indicating that the session is still going, the initial packet and it's recepient is flagged as hacked and the packet is ignored. This will trigger a warning to the user in UI, and rules may be set up in plan to automatically shut down communication uppon such an event.
						+ No matter if bandwidth management is in effect or not, valid initial packets should be processed at a rate at most once per 3 seconds.
		*/
	//} else {
	//}

	if(recieveMagicAndVersion(state)) {
		state.readProtocolEncryptedMessage();
		if(state.octomyProtocolEncryptedMessageSize <= 0) {
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol encrypted message size <= 0";
			qWarning()<<es;
			emit commsError(es);
			return;
		}
		state.readEncSenderID();
		// TODO meta overhead bytes for bytearray from stream
		if(state.octomyProtocolSenderIDRawSize != state.OCTOMY_SENDER_ID_SIZE) { // Full ID should be OCTOMY_FULL_ID_SIZE bytes, no more, no less
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less Sender ID size mismatch: "+QString::number(state.octomyProtocolSenderIDRawSize)+ " vs. "+QString::number(state.OCTOMY_SENDER_ID_SIZE);
			qWarning()<<es;
			emit commsError(es);
			return;
		}
		qDebug()<< "SESSION-LESS PACKET RX FROM "<< state.octomyProtocolSenderID;
		state.readEncRemoteNonce();
		qDebug()<< "SYN REMOTE NONCE WAS: "<< state.octomyProtocolRemoteNonce;


		state.readEncDesiredRemoteSessionID();
		if(state.octomyProtocolDesiredRemoteSessionID < FIRST_STATE_ID ) {
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol desired remote session ID not valid: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
			qWarning()<<es;
			emit commsError(es);
			return;
		}

		auto session=lookUpSession(state.octomyProtocolSenderID,state.octomyProtocolDesiredRemoteSessionID);
		if(nullptr!=session) {
			qDebug()<<"SESSION: "<<session->address().toString()<<QStringLiteral(", ")<<session->key().id();
		}
	}
}

QSharedPointer<CommsSession> CommsChannel::lookUpSession(QString id, SESSION_ID_TYPE desiredRemoteSessionID)
{
	// Look for existing sessions tied to this ID
	QSharedPointer<CommsSession> session=mSessions.getByFullID(id);
	if(nullptr==session) {
		if(mKeystore.hasPubKeyForID(id)) {
			Key key=mKeystore.pubKeyForID(id);
			if(key.isValid(true)) {
				SESSION_ID_TYPE localSessionID=mSessions.generateUnusedSessionID();
				if(localSessionID < FIRST_STATE_ID ) {
					QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol local session ID not valid: "+QString::number(localSessionID);
					qWarning()<<es;
					emit commsError(es);
				} else {
					QSharedPointer<NodeAssociate> participant=mPeers.getParticipant(id);
					if(nullptr==participant) {
						QString es=QString::number(sTotalRecCount)+" ERROR: no participant found for ID "+id;
						qWarning()<<es;
						emit commsError(es);
					} else {
						session=QSharedPointer<CommsSession>(new CommsSession(key));
						session->setRemoteSessionID(desiredRemoteSessionID);
						session->setLocalSessionID(localSessionID);
						session->setAddress(participant->publicAddress());
						mSessions.insert(session);
						qDebug()<< "NEW SESSION CREATED FOR ID "<<id<< " with remote sessionID "<<QString::number(desiredRemoteSessionID);
					}
				}
			} else {
				QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Could not create session for sender	with ID "+id+", key was invalid";
				qWarning()<<es;
				emit commsError(es);
			}
		} else {
			//Unknown sender
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less sender unknown";
			qWarning()<<es;
			emit commsError(es);
		}
	}
	return session;
}

// 2. B answers SYN-ACK to A: Hi A. Here is my ( FULL-ID + NEW NONCE + RETURN NONCE + B's DESIRED SESSION-ID ) ENCODED WITH YOUR PUBKEY back atch'a.
void CommsChannel::recieveSynAck(PacketReadState &state)
{
	qDebug()<<"RECEIVED SYN-ACK PACKET";


	if(recieveMagicAndVersion(state)) {
		// Extract remote nonce
		state.readEncRemoteNonce();
		qDebug()<< "SYN-ACK REMOTE NONCE WAS: "<< state.octomyProtocolRemoteNonce;

		// Extract return nonce
		state.readEncReturnNonce();
		qDebug()<< "SYN-ACK RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce;


		// Extract desired remote session ID
		state.readEncDesiredRemoteSessionID();
		if(state.octomyProtocolDesiredRemoteSessionID < FIRST_STATE_ID ) {
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol desired remote session ID not valid: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
			qWarning()<<es;
			emit commsError(es);
			return;
		}

	}
}


// 3. A answers ACK to B: Hi again B. Here is my ( FULL-ID + RETURN NONCES ) ENCODED WITH YOUR PUBKEY back atch'a.
void CommsChannel::recieveAck(PacketReadState &state)
{
	qDebug()<<"RECEIVED ACK PACKET";

	if(recieveMagicAndVersion(state)) {
		// Extract return nonce
		state.readEncReturnNonce();
		//TODO: Verify that the return nonce was OK
		//if(state.octomyProtocolReturnNonce !=  bla bla)
		qDebug()<< "ACK RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce;
	}

}






void CommsChannel::recieveData(PacketReadState &state)
{

	SESSION_ID_TYPE sessionID = (SESSION_ID_TYPE)state.multimagic;
	if(sessionID < FIRST_STATE_ID) {
		QString es=QString::number(sTotalRecCount)+" ERROR: invalid multimagic specified";
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	auto session=mSessions.getBySessionID(sessionID);
	if(nullptr==session) {
		//Unknown sender
		QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Full sender unknown for session-ID: '"+QString::number(sessionID)+"'";
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	qDebug()<<"SESSIONFULL CORRESPONDANT: "<<session->address().toString()<<", "<<session->key().id()<< QStringLiteral(" sid:")<< (sessionID);

	/*
				static const int expectedHeaderSize = sizeof(state.octomyProtocolMagic)+sizeof(state.octomyProtocolFlags)+sizeof(state.octomyProtocolVersion)+sizeof(state.remoteSessionID)
	#ifdef USE_RELIBABILITY_SYSTEM
													  +sizeof(packet_sequence)+sizeof(packet_ack)+sizeof(packet_ack_bits)
	#endif
													  ;//+sizeof(quint32 );
	//qDebug()<<totalRecCount<<"PACKET INITIAL SIZE: "<<size<<", HEADER CALCULATED SIZE: "<<header<<", THUS RAW BYES EXPECTED: "<<(size-header);
				if ( size <= expectedHeaderSize  ) {
					QString es=QString::number(sTotalRecCount)+" ERROR: Message too short: " +QString::number(size)+" vs. header: "+QString::number(expectedHeaderSize );
					qWarning()<<es;
					emit commsError(es);
					return;
				}
	*/

	session->receive();
	quint16 partsCount=0;
	const qint32 minAvailableForPart  = ( sizeof(quint32)  );
	if(state.totalAvailable < minAvailableForPart) {
		qWarning()<<"ERROR: NO PARTS DETECTED! Data size too small";
	}
	while(state.totalAvailable >= minAvailableForPart) {
		partsCount++;
		//qDebug()<<"READING PART #"<<partsCount<<" WITH "<<totalAvailable<<" vs. "<<minAvailableForPart;
		state.readPartMessageTypeID();
		state.readPartBytesAvailable();
		// Is this an internal part?
		if(state.partMessageTypeID < Courier::FIRST_USER_ID) {
			//Use message type enum for built in messages
			const MessageType partMessageType=(MessageType)state.partMessageTypeID;
			//qDebug()<<totalRecCount<<"MESSAGE TYPE WAS"<<partMessageType<<"("<<QString::number(partMessageTypeID)<<")";
			switch(partMessageType) {
			case(IDLE): {
				qDebug()<<sTotalRecCount<<"GOT IDLE";
				// Idle packet does not contain any data, so we are done here :)
			}
			break;
			default:
			case(INVALID): {
				QString es=QString::number(sTotalRecCount)+" "+QString::number(partsCount)+" ERROR: OctoMY message type invalid: "+QString::number((quint32)partMessageType,16);
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			break;
			}
		} else {
			//Use courier id for extendable messages
			Courier *courier=getCourierByID(state.partMessageTypeID);
			if(nullptr!=courier) {
				const quint16 bytesSpent=courier->dataReceived(*state.stream, state.partBytesAvailable);
				const int left=state.partBytesAvailable-bytesSpent;
				//qDebug()<<totalRecCount<<"GOT COURIER MSG "<<partMessageTypeID<<" "<<c->name()<<" bytes avail="<<partBytesAvailable<<" tot avail="<<totalAvailable<<" bytes spent="<<bytesSpent<<" left="<<left<<"  ";
				state.totalAvailable-=bytesSpent;
				if(left>=0) {
					if(left>0) {
						qWarning()<<sTotalRecCount<<"WARNING: SKIPPING "<<left<<" LEFTOVER BYTES AFTER COURIER WAS DONE";
						state.stream->skipRawData(left);
						state.totalAvailable-=left;
					} else {
						//qDebug()<<totalRecCount<<"ALL GOOD. COURIER BEHAVED EXEMPLARY";
					}
				} else {
					QString es=QString::number(sTotalRecCount)+" "+QString::number(partsCount)+" ERROR: Courier read more than was available!";
					qWarning()<<es;
					emit commsError(es);
					return;
				}
				//qDebug()<<"tot Available:"<<totalAvailable<<" available:"<<partBytesAvailable<<" dataStream at end:"<<ds->atEnd();
			} else {
				//TODO: Look at possibility of registering couriers on demand using something like this:
				//emit wakeOnComms(octomy_message_type_int)
				QString es=QString::number(sTotalRecCount)+" "+QString::number(partsCount)+" ERROR: No courier found for ID: "+QString::number(state.partMessageTypeID);
				qWarning().noquote()<<es;
				emit commsError(es);
				return;
			}
			//qDebug()<<totalRecCount<<partsCount<<"PART DONE";
		}
	}
	if(state.totalAvailable > 0 ) {
		QString es="WARNING: Not all bytes in datagram were read/used. There were "+QString::number(state.totalAvailable)+" bytes left after reading "+QString::number(partsCount)+" parts";
		qWarning()<<es;
		return;
	}
}



// Main packet reception
void CommsChannel::receivePacketRaw( QByteArray datagramS, QHostAddress remoteHostS , quint16 remotePortS )
{
	mRXRate.countPacket(datagramS.count());
	//countReceived();
	PacketReadState state(datagramS, remoteHostS,remotePortS);

	QSharedPointer<CommsSession> session;
	state.readMultimagic();
	switch((Multimagic)state.multimagic) {
	case(MULTIMAGIC_IDLE): {
		// We are keeping the UDP connection alive with idle packet
		recieveIdle(state);
	}
	break;
	case(MULTIMAGIC_SYN): {
		// We received SYN
		if(recieveEncryptedBody(state)) {
			recieveSyn(state);
		}
	}
	break;
	case(MULTIMAGIC_SYNACK): {
		// We received SYN-ACK
		if(recieveEncryptedBody(state)) {
			recieveSynAck(state);
		}
	}
	break;
	case(MULTIMAGIC_ACK): {
		// We received SYN
		if(recieveEncryptedBody(state)) {
			recieveAck(state);
		}
	}
	break;

	// No special purpose packet type was specified
	default: {
		recieveData(state);
	}
	break;
	}

}



void CommsChannel::doSend( PacketSendState &state)
{
	// Send data in stream
	const quint32 sz=state.datagram.size();
	if(nullptr==state.session) {
		qWarning()<<"ERROR: session was null";
		return;
	}
	const NetworkAddress na=state.session->address();
	if(!na.isValid(false,false)) {
		qWarning()<<"ERROR: invalid address: "	<<na;
		return;
	}
	if(state.datagram.size()<=0) {
		qWarning()<<"ERROR: datagram is <= 0 bytes ("<<state.datagram.size() <<")";
		return;
	}

	const qint64 written=mUDPSocket.writeDatagram(state.datagram, na.ip(), na.port());
	//qDebug()<<"WROTE "<<written<<" bytes to "<<sig;
	if(written<0) {
		qDebug()<<"ERROR: in write for UDP SOCKET:"<<mUDPSocket.errorString()<< " for destination "<< mLocalAddress.toString()<<localID();
		return;
	} else if(written<sz) {
		qDebug()<<"ERROR: Only " << written << " of " <<sz<<" bytes of idle packet written to UDP SOCKET:"<<mUDPSocket.errorString()<< " for destination "<< mLocalAddress.toString()<<localID();
		return;
	} else {
		qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
	}
	if (nullptr!=state.session) {
		state.session->countSend(written);
	}
	mTXRate.countPacket(written);
	sTotalTxCount++;
}

////////////////////////////////////////////////////////////////////////////////

void CommsChannel::sendIdle(const quint64 &now, QSharedPointer<CommsSession> session)
{
	if(nullptr==session) {
		qWarning()<<"ERROR: session was null";
		return;
	}
	PacketSendState state;

	auto sessionID=session->remoteSessionID();
	// TODO: What tha hall
	state.writeSessionID(sessionID);

	doSend(state);
}



////////////////////////////////////////////////////////////////////////////////

void CommsChannel::sendSyn(PacketSendState &state)
{
	// 1. Send Alice's magic & version
	state.writeMagic();
	state.writeProtocolVersion();

	// 2. Send encrypted package of...

	// 2a. Alice's full sender ID
	auto senderID=localID();
	state.writeEncSenderID(senderID);
	//TODO: We should create new session here and make sure there is no cross talk
	// 2b. Alice's first nonce
	// 2c. Alice's desired session ID

	state.writeProtocolEncryptedMessage();

	doSend(state);
}



////////////////////////////////////////////////////////////////////////////////

void CommsChannel::sendSynAck(PacketSendState &state)
{
	// 1. Send Bob's magic & version
	state.writeMagic();
	state.writeProtocolVersion();

	// 2. Send encrypted package of...

	// 2a. Bob's first nonce
	// 2b. Alice's first return nonce
	// 2c. Bob's desired session ID

	doSend(state);
}

////////////////////////////////////////////////////////////////////////////////

void CommsChannel::sendAck(PacketSendState &state)
{
	// 1. Send Alice's magic & version
	state.writeMagic();
	state.writeProtocolVersion();

	// 2. Send encrypted package of...
	// 2a. Bob's first return nonce

	doSend(state);
}



////////////////////////////////////////////////////////////////////////////////

void CommsChannel::sendData(const quint64 &now, Courier &courier)
{
	QString toID=courier.destination();
	if(toID.isEmpty()) {
		qWarning()<<"ERROR: Invalid courier destination when sending data";
		return;
	}

	QSharedPointer<CommsSession> session=lookUpSession(toID,0);
	if(nullptr==session) {
		qWarning()<<"ERROR: No session found for courier with destination "<<toID;
		return;
	}

	PacketSendState state;

	state.setSession(session);

	if(session->established()) {
		// Ladies, the bar is open!

	} else {
		HandshakeStep step=session->nextStep();
		switch(step) {
		default:
		case(VIRGIN): {
			qWarning()<<"ERROR: Unknown session step "<<QString::number(step);
		}
		break;
		case(SYN_OK): {
			sendSyn(state);
		}
		break;
		case(SYN_ACK_OK): {
			sendSynAck(state);
		}
		break;
		case(ACK_OK): {
			sendAck(state);
		}
		break;

		}
	}


	const qint32 availableBytes = MAX_UDP_PAYLOAD_SIZE - ( 7 * 4 );


	state.writeMagic();
	state.writeProtocolVersion();

	auto sessionID=session->remoteSessionID();
	state.writeSessionID(sessionID);

#ifdef USE_RELIBABILITY_SYSTEM
	ReliabilitySystem &rs=localClient->reliabilitySystem();
	state.writeReliabilityData(rs);
#endif

	const quint32 courierID=courier.id();
	state.writeCourierID(courierID);

	const quint16 payloadSize=courier.mandate().payloadSize;
	state.writePayloadSize(payloadSize);

	//qDebug()<<"SEND LOCAL HEADER SIZE: "<<bytesUsed<<datagram.size();
	const quint64 opportunityBytes=courier.sendingOpportunity(*state.stream);
	state.bytesUsed += opportunityBytes;
	//qDebug()<<"SEND FULL SIZE: "<<bytesUsed<<datagram.size()<<opportunityBytes;
	courier.setLastOpportunity(now);

	const quint32 sz=state.datagram.size();
	if(sz>MAX_UDP_PAYLOAD_SIZE) {
		qWarning()<<"ERROR: UDP packet size exceeded " <<( MAX_UDP_PAYLOAD_SIZE)<<" bytes which is implemented as practical fragment-free UDP payload size, dropping write";
	} else if( (availableBytes - state.bytesUsed) <= 0) {
		qWarning()<<"ERROR: courier trying to send too much data: "<<QString::number(state.bytesUsed)<<" , dropping write";
	} else {
		doSend(state);
	}
}




quint64 CommsChannel::rescheduleSending(quint64 now)
{
	mTXScheduleRate.countPacket(0);
	//qDebug()<<" ... R E S C H E D U L E ... "<<now;
	mMostUrgentCourier=MINIMAL_PACKET_RATE;
	mSchedule.clear();
	// Update first
	for(Courier *courier:mCouriers) {
		if(nullptr!=courier) {
			courier->update(now);
		}
	}
	// Re-schedule couriers (strive to adhere to the mandates set fourth by each courier)
	for(Courier *courier:mCouriers) {
		if(nullptr!=courier) {
			CourierMandate cm=courier->mandate();
			//qDebug()<<"MANDATE FOR "<<courier->name()<<" IS "<<cm.toString();
			if(cm.sendActive) {
				//const QString &courierDestination=courier->destination();
				const qint64 timeLeft = cm.nextSend - now;
				// Update most-urgent time
				if(timeLeft < mMostUrgentCourier) {
					const quint64 lastUrgent=mMostUrgentCourier;
					mMostUrgentCourier=qMax(timeLeft, 0LL);
					qDebug()<<courier->name()<<courier->id()<<courier->ser()<<" WITH TIME LEFT " << timeLeft<< " BUMPED URGENT FROM "<<lastUrgent<<" -> "<<mMostUrgentCourier;
				}
				// Actual sending is due, schedule a packet to associate of client
				if(timeLeft < 0) {
					const quint64 score=(cm.priority * -timeLeft );
					mSchedule.insert(score, courier); //TODO: make this broadcast somehow (use ClientDirectory::getByLastActive() and ClientSignature::isValid() in combination or similar).
					qDebug()<<courier->name()<<courier->id()<<courier->ser()<<" SCHEDULED WITH TIMELEFT "<<timeLeft<<" AND SCORE "<<score;
				}
				/*
				// Courier has been dormant past idle time, schedule an idle packet to courier
				else if(now-courier->lastOpportunity() > MINIMAL_PACKET_RATE) {
					mIdle.insert(&courierDestination);
					qDebug()<<courier->name()<<courier->id()<<courier->ser()<<" SCHEDULED FOR IDLE";
				}
				*/
			}
		}
	}
	// Prepare for next sending opportunity
	const quint64 delay=qBound((qint64) 0, mMostUrgentCourier, (qint64)MINIMAL_PACKET_RATE);
	qDebug()<<"NEW SCHEDULE DELAY: "<<delay<<" ("<<mMostUrgentCourier<<")";
	mSendingTimer.setInterval(delay);
	return delay;
}

void CommsChannel::detectConnectionChanges(const quint64 now)
{
	const quint64 timeSinceLastRX=now-mRXRate.mLast;

	if(mConnected && (timeSinceLastRX > CONNECTION_TIMEOUT) ) {
		qDebug()<<"Connection timed out, stopping";
		stop();
	} else if(!mConnected && (timeSinceLastRX <= CONNECTION_TIMEOUT) ) {
		mConnected=true;
		qDebug()<<"Connection completed";
		emit commsConnectionStatusChanged(true);
	}
}



//////////////////////////////////////////////////
// Send & receive slots

void CommsChannel::onSendingTimer()
{
	qDebug()<<"--- SEND";
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	detectConnectionChanges(now);
	mTXOpportunityRate.countPacket(0);

	qDebug()<<"SCHEDULE: "<<mSchedule.size();
	for (QMap<quint64, Courier *>::iterator i = mSchedule.begin(); i != mSchedule.end(); ) {
		Courier *courier=i.value();
		qDebug() << " + " << i.key() << " = " << ((nullptr==courier)?"NULL":courier->toString());
		++i;
	}
	// Send data according to schedule
	for (QMap<quint64, Courier *>::iterator i = mSchedule.begin(); i != mSchedule.end(); ) {
		Courier *courier=i.value();
		if(nullptr!=courier) {
			sendData(now, *courier);
		} else {
			qWarning()<<"WARNING: scheduled courier was null";
		}
		++i;
	}
	// Send idle packets to sessions that need it
	auto idle=mSessions.getByIdleTime(MINIMAL_PACKET_RATE);
	const int isz=idle.size();
	if(isz>0) {
		qDebug()<<"Sending idle packets to "<<isz<<" sessions in need";
		for(QSharedPointer<CommsSession> session:idle) {
			if(nullptr!=session) {
				sendIdle(now, session);
			} else {
				qWarning()<<"ERROR: session was null when sending idle packet";
			}
		}
	} else {
		qDebug()<<"No sessions in need of idle packets this time";
	}
	rescheduleSending(now);
}

// NOTE: This is only for testing purposes! Real data should pass through the courier system
// and be dispatched by logic in sending timer
qint64 CommsChannel::sendRawData(QByteArray datagram, NetworkAddress address)
{
	return mUDPSocket.writeDatagram(datagram, address.ip(), address.port());
}

NetworkAddress CommsChannel::localAddress()
{
	return mLocalAddress;
}

QString CommsChannel::localID()
{
	return mKeystore.localKey().id();
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
	qDebug()<<"COMMS_CHANNEL_LOG: "<<msg;
}

/*
void CommsChannel::setID(const QString &id)
{
	mLocalID=id;

}
*/

void CommsChannel::setHookCommsSignals(QObject &ob, bool hook)
{
	qRegisterMetaType<CommsSession *>("Client *");
	if(hook) {
		if(!connect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(commsClientAdded(CommsSession *)),&ob,SLOT(onCommsClientAdded(CommsSession *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(commsConnectionStatusChanged(bool)),&ob,SLOT(onCommsConnectionStatusChanged(bool)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(commsClientAdded(CommsSession *)),&ob,SLOT(onCommsClientAdded(CommsSession *)))) {
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
	Q_ASSERT(this==&courier.comms());
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
