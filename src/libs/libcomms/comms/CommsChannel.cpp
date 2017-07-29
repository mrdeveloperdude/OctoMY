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

QString MultimagicToString(Multimagic m)
{
#define MULTIMAGIC_TO_STRING_CASE(a) case(a): return(#a)
	switch(m) {
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_IDLE);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_SYN);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_SYNACK);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_ACK);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_LAST);
	default:
		return "MULTIMAGIC_"+QString::number((SESSION_ID_TYPE)m);

	}
#undef MULTIMAGIC_TO_STRING_CASE
}

QString MultimagicToString(SESSION_ID_TYPE m)
{
	return MultimagicToString((Multimagic)m);
}


//#define DO_CC_ENC

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
	mSendingTimer.setSingleShot(false);
	mSendingTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&mSendingTimer, SIGNAL(timeout()), this, SLOT(onSendingTimer()), OC_CONTYPE)) {
		qWarning()<<"Could not connect sending timer";
	}
}

CommsChannel::CommsChannel(KeyStore &keystore, QObject *parent)
	: QObject(parent)
	, mKeystore(keystore)
	, mPeers(*((NodeAssociateStore *) nullptr ))//HACK
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
	return (mSendingTimer.isActive() && (mUDPSocket.state()==QAbstractSocket::BoundState));
}

bool CommsChannel::isConnected() const
{
	return mConnected;
}

////////////////////////////////////////////////////////////////////////////////

struct PacketSendState {
public:
	QString mRemoteID;
	QSharedPointer<CommsSession> session;
	QByteArray datagram;
	QSharedPointer<QDataStream> stream;
	int bytesUsed;
	QSharedPointer<QDataStream> encStream;
	int encBytesUsed;
	QByteArray octomyProtocolUnencryptedMessage;
	QByteArray octomyProtocolEncryptedMessage;



	static const int OCTOMY_SENDER_ID_SIZE=20;

public:
	PacketSendState()
		: session(nullptr)
		, stream(new QDataStream(&this->datagram, QIODevice::WriteOnly))
		, bytesUsed(0)
		, encStream(new QDataStream(&this->octomyProtocolUnencryptedMessage, QIODevice::WriteOnly))
		, encBytesUsed(0)
	{

	}

	void setRemoteID(QString toID)
	{
		mRemoteID=toID;
	}

	void setSession( QSharedPointer<CommsSession> session)
	{
		this->session=session;
	}

	void writeMultimagic(Multimagic multimagic)
	{
		const SESSION_ID_TYPE multimagicInt=(SESSION_ID_TYPE)multimagic;
		writeMultimagic(multimagicInt);
	}

	void writeMultimagic(SESSION_ID_TYPE multimagic)
	{
		*stream << multimagic;
		bytesUsed += sizeof(multimagic);
	}

	void writeMagic()
	{
		const quint32 magic=(quint32)OCTOMY_PROTOCOL_MAGIC;//Protocol MAGIC identifier
		*stream << magic;
		bytesUsed += sizeof(magic);
	}

	// Write a header with protocol "magic number" and a version
	void writeProtocolVersion()
	{
		const quint32 version=(qint32)OCTOMY_PROTOCOL_VERSION_CURRENT;//Protocol version
		*stream << version;
		bytesUsed += sizeof(version);
		stream->setVersion(version); //Qt Datastream version
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

	void encrypt()
	{
		if(nullptr==session) {
			qWarning()<<"ERROR: No session while encrypting";
			return;
		}
		auto k=session->key();
		if(nullptr==k) {
			qWarning()<<"ERROR: No key while encrypting";
			return;
		}
		if(!k->isValid(true)) {
			qWarning()<<"ERROR: Invalid key while encrypting";
			return;
		}
		if(octomyProtocolUnencryptedMessage.size()<1) {
			qWarning()<<"ERROR: Source text is empty while encrypting";
			return;
		}
#ifdef DO_CC_ENC
		octomyProtocolEncryptedMessage=k.encrypt(octomyProtocolUnencryptedMessage);
#else
		octomyProtocolEncryptedMessage=octomyProtocolUnencryptedMessage;
#endif
	}


	// Write Pub-key encrypted message body
	void writeProtocolEncryptedMessage()
	{
		auto size=octomyProtocolEncryptedMessage.size();
		qWarning()<<"TX CIPHERTEXT WAS: "<<octomyProtocolEncryptedMessage;
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
	void writeEncNonce(SESSION_NONCE_TYPE nonce)
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
	QByteArray octomyProtocolDecryptedMessage;
	int octomyProtocolEncryptedMessageSize;
	int octomyProtocolDecryptedMessageSize;

	quint32 partMessageTypeID;
	quint16 partBytesAvailable;



	QSharedPointer<QDataStream> encStream;
	int encTotalAvailable;


	QByteArray octomyProtocolSenderIDRaw;
	int octomyProtocolSenderIDRawSize;
	QString octomyProtocolSenderID;

	SESSION_NONCE_TYPE octomyProtocolRemoteNonce;
	SESSION_NONCE_TYPE octomyProtocolReturnNonce;

	SESSION_ID_TYPE octomyProtocolDesiredRemoteSessionID;

	static const int OCTOMY_SENDER_ID_SIZE=32;

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
		, octomyProtocolDecryptedMessageSize(0)
		, partMessageTypeID(0)
		, partBytesAvailable(0)
		, encStream()
		, encTotalAvailable(0)
		, octomyProtocolSenderIDRawSize(0)
		, octomyProtocolRemoteNonce(INVALID_NONCE)
		, octomyProtocolReturnNonce(INVALID_NONCE)
		, octomyProtocolDesiredRemoteSessionID(INVALID_SESSION_ID)
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

	static const int OCTOMY_ENCRYPTED_MESSAGE_SIZE=36;


	void decrypt(Key &k)
	{

		if(k.isValid(true)) {
			if(octomyProtocolEncryptedMessageSize>0) {
#ifdef DO_CC_ENC
				octomyProtocolDecryptedMessage=k.decrypt(octomyProtocolEncryptedMessage);
#else
				octomyProtocolDecryptedMessage=octomyProtocolEncryptedMessage;
#endif
				octomyProtocolDecryptedMessageSize=octomyProtocolDecryptedMessage.size();
				if(octomyProtocolDecryptedMessageSize>0) {
					encStream=QSharedPointer<QDataStream> (new QDataStream(&this->octomyProtocolDecryptedMessage, QIODevice::ReadOnly));
					encTotalAvailable=octomyProtocolEncryptedMessageSize;
				} else {
					qWarning()<<"ERROR: Source text is empty while decrypting";
				}
			} else {
				qWarning()<<"ERROR: Cipher text is empty while decrypting";
			}
		} else {
			qWarning()<<"ERROR: Invalid key while decrypting";
		}


	}

	// Read Pub-key encrypted message body
	void readProtocolEncryptedMessage()
	{
		*stream >> octomyProtocolEncryptedMessage;
		qWarning()<<"RX CIPHERTEXT WAS: "<<octomyProtocolEncryptedMessage;
		octomyProtocolEncryptedMessageSize=octomyProtocolEncryptedMessage.size();
		totalAvailable-=octomyProtocolEncryptedMessageSize;
	}


	// Extract full ID of sender
	void readEncSenderID()
	{
		octomyProtocolSenderIDRaw.clear();
		*encStream >> octomyProtocolSenderIDRaw;
		octomyProtocolSenderIDRawSize=octomyProtocolSenderIDRaw.size();
		encTotalAvailable-=octomyProtocolSenderIDRawSize;
		octomyProtocolSenderID=octomyProtocolSenderIDRaw.toHex().toUpper();
	}


	// Extract remote nonce
	void readEncRemoteNonce()
	{
		octomyProtocolRemoteNonce=INVALID_NONCE;
		*encStream >> octomyProtocolRemoteNonce;
		encTotalAvailable-=sizeof(octomyProtocolRemoteNonce);
	}


	// Extract return nonce
	void readEncReturnNonce()
	{
		octomyProtocolReturnNonce=INVALID_NONCE;
		*encStream >> octomyProtocolReturnNonce;
		encTotalAvailable-=sizeof(octomyProtocolReturnNonce);
	}


	// Extract desired remote session ID
	void readEncDesiredRemoteSessionID()
	{
		octomyProtocolDesiredRemoteSessionID=INVALID_SESSION_ID;
		*encStream >> octomyProtocolDesiredRemoteSessionID;
		encTotalAvailable-=sizeof(octomyProtocolDesiredRemoteSessionID);
	}

};


bool CommsChannel::recieveEncryptedBody(PacketReadState &state)
{
	QSharedPointer<Key> localKey=mKeystore.localKey();
	if(nullptr!=localKey) {
		// Decrypt message body using local private-key
		if(!localKey->isValid(false)) {
			QString es="ERROR: Local key not valid for decryption";
			qWarning()<<es;
			emit commsError(es);
			return false;
		}
		// Read Pub-key encrypted message body
		state.readProtocolEncryptedMessage();
		//state.decrypt(*localKey);
		/*
		if(state.octomyProtocolEncryptedMessageSize != state.OCTOMY_ENCRYPTED_MESSAGE_SIZE) { // Encrypted message should be OCTOMY_ENCRYPTED_MESSAGE_SIZE bytes, no more, no less
			QString es="ERROR: OctoMY Protocol Session-Less encrypted message size mismatch: "+QString::number(state.octomyProtocolEncryptedMessageSize)+ " vs. "+QString::number(state.OCTOMY_ENCRYPTED_MESSAGE_SIZE);
			qWarning()<<es;
			emit commsError(es);
			return false;
		}
		*/
		state.decrypt(*localKey);
		//const int octomyProtocolDecryptedMessageSize=

		if(0 == state.octomyProtocolDecryptedMessageSize) { // Size of decrypted message should be non-zero
			QString es="ERROR: OctoMY Protocol Session-Less decryption failed with local key with ID: " + localKey->id();
			qWarning()<<es;
			qWarning()<<"RX CIPHERTEXT WAS: "<<state.octomyProtocolDecryptedMessage;
			emit commsError(es);
			return false;
		}
		return true;
	} else {
		QString es="ERROR: no key";
		qWarning()<<es;
		emit commsError(es);
	}
	return false;
}

bool CommsChannel::recieveMagicAndVersion(PacketReadState &state)
{
	state.readProtocolMagic();
	if(OCTOMY_PROTOCOL_MAGIC!=state.octomyProtocolMagic) {
		QString es="ERROR: OctoMY Protocol Magic mismatch: "+QString::number(state.octomyProtocolMagic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16);
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




QSharedPointer<CommsSession> CommsChannel::createSession(QString id, bool initiator)
{
	QSharedPointer<CommsSession> session(nullptr);
	if(mKeystore.hasPubKeyForID(id)) {
		auto key=mKeystore.pubKeyForID(id);
		if(nullptr!=key) {
			if(key->isValid(true)) {
				SESSION_ID_TYPE localSessionID=mSessions.generateUnusedSessionID();
				if(localSessionID < FIRST_STATE_ID ) {
					QString es="ERROR: OctoMY Protocol local session ID not valid: "+QString::number(localSessionID);
					qWarning()<<es;
					emit commsError(es);
				} else {
					QSharedPointer<NodeAssociate> participant=mPeers.getParticipant(id);
					if(nullptr==participant) {
						QString es="ERROR: no participant found for ID "+id;
						qWarning()<<es;
						emit commsError(es);
					} else {
						session=QSharedPointer<CommsSession>(new CommsSession(key));
						OC_ASSERT(nullptr!=session);
						if(nullptr!=session) {
							//session->setRemoteSessionID(desiredRemoteSessionID);
							session->handshakeState().setInitiator(initiator);
							session->setLocalSessionID(localSessionID);
							session->setAddress(participant->publicAddress());
							mSessions.insert(session);
							qDebug()<< "NEW SESSION CREATED FOR ID "<<id<< " with local sessionID "<<QString::number(localSessionID)<< (initiator?"INITIATED":"");

							//qDebug()<<"CREATE SESSION RETURNING VALUE: "<<session->summary();
							/*
							auto k=session->key();
							if(nullptr!=k) {
								qDebug()<<"IN:  KEY PTR="<<(void *)(k.data())<<", DKEY PTR="<<(void *)(k->d_func_dbg() );
							} else {
								qDebug()<<"IN:  NO KEY";
							}
							*/

						} else {
							QString es="ERROR: OctoMY Protocol Could not create session for sender with ID "+id+", could not allocate";
							qWarning()<<es;
							emit commsError(es);
						}
					}
				}
			} else {
				QString es="ERROR: OctoMY Protocol Could not create session for sender with ID "+id+", key was invalid";
				qWarning()<<es;
				emit commsError(es);
			}
		} else {
			QString es="ERROR: no key";
			qWarning()<<es;
			emit commsError(es);
		}
	} else {
		//mKeystore.dump();
		QString es=QStringLiteral("ERROR: OctoMY Protocol Session-Less sender '")+id+QStringLiteral("' unknown. (Keystore.ready=")+QString(mKeystore.isReady()?"YES":"NO")+")";
		qWarning().noquote()<<es;
		emit commsError(es);
	}
	return session;
}


QSharedPointer<CommsSession> CommsChannel::lookUpSession(QString id)
{
	// Look for existing sessions tied to this ID
	QSharedPointer<CommsSession> session=mSessions.getByFullID(id);
	return session;
}



void CommsChannel::recieveIdle(PacketReadState &state)
{
	qDebug()<<"RECEIVED IDLE PACKET";
}



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


/*
1. A sends SYN to B:
	Hi B. Here is my
	  + FULL-ID
	  + DESIRED SESSION-ID
	  + NONCE
	ENCODED WITH YOUR(B's) PUBKEY.
*/
void CommsChannel::recieveSyn(PacketReadState &state)
{
	// REMOTE FULL-ID
	state.readEncSenderID();
	qDebug()<<"SYN RX FROM "<< state.octomyProtocolSenderID;
	// TODO meta overhead bytes for bytearray from stream
	if(state.octomyProtocolSenderIDRawSize != state.OCTOMY_SENDER_ID_SIZE) { // Full ID should be OCTOMY_FULL_ID_SIZE bytes, no more, no less
		QString es="ERROR: OctoMY Protocol Session-Less Sender ID size mismatch: "+QString::number(state.octomyProtocolSenderIDRawSize)+ " vs. "+QString::number(state.OCTOMY_SENDER_ID_SIZE);
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	// Look up session for remote full ID
	auto session=lookUpSession(state.octomyProtocolSenderID);
	if(nullptr!=session) {
		QString es="ERROR: OctoMY Protocol Session already existed for: " + state.octomyProtocolSenderID;
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	// Create session for remote full ID
	session=createSession(state.octomyProtocolSenderID, false);
	if(nullptr==session) {
		QString es="ERROR: OctoMY Protocol Session could not be created for: " + state.octomyProtocolSenderID;
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	//TODO: Remove session unless handshake bump happens to avoid endless fail loop
	/*
	auto k=session->key();
	if(nullptr!=k) {
		//qDebug()<<"OUT: KEY PTR="<<(void *)(k.data())<<", DKEY PTR="<<(void *)(k->d_func_dbg() );
		//qDebug()<<"ID= "<<state.octomyProtocolSenderID<<", KEY: "<<k->id();
		//qDebug()<<"RX SYN SESSION: "<<session->address().toString()<<QStringLiteral(", ")<<k->id();
	} else {
		qDebug()<<"ERROR: No key";
	}
	*/

	// REMOTE DESIRED SESSION-ID
	state.readEncDesiredRemoteSessionID();
	if(state.octomyProtocolDesiredRemoteSessionID < FIRST_STATE_ID ) {
		QString es="ERROR: OctoMY Protocol desired remote session ID not valid for SYN: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	qDebug()<< "SYN RX REMOTE DESIRED SESSION-ID WAS: "<< state.octomyProtocolDesiredRemoteSessionID;
	// REMOTE NONCE
	state.readEncRemoteNonce();
	if(state.octomyProtocolRemoteNonce == INVALID_NONCE) {
		QString es="ERROR: OctoMY Protocol remote nonce was invalid for SYN: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	qDebug()<< "SYN RX REMOTE NONCE WAS: "<< state.octomyProtocolRemoteNonce;

	// Record remote nonce
	session->setTheirLastNonce(state.octomyProtocolRemoteNonce);
	// Record remote desired session id
	session->setRemoteSessionID(state.octomyProtocolDesiredRemoteSessionID);

	session->handshakeState().setSynOK();
	qDebug()<< "SYN RX handshake state is now :"<< handshakeStepToString(session->handshakeState().step());
}


/*
2. B answers SYN-ACK to A:
	Hi A. HERE IS MY
	  + FULL-ID
	  + DESIRED SESSION-ID
	  + RETURN NONCE
	  + NEW NONCE
	ENCODED WITH YOUR(A's) PUBKEY back atch'a.
*/
void CommsChannel::recieveSynAck(PacketReadState &state)
{
	// REMOTE FULL-ID
	state.readEncSenderID();
	qDebug()<<"SYN-ACK RX FROM "<< state.octomyProtocolSenderID;
	// TODO meta overhead bytes for bytearray from stream
	if(state.octomyProtocolSenderIDRawSize != state.OCTOMY_SENDER_ID_SIZE) { // Full ID should be OCTOMY_FULL_ID_SIZE bytes, no more, no less
		QString es="ERROR: OctoMY Protocol Session-Less Sender ID size mismatch: "+QString::number(state.octomyProtocolSenderIDRawSize)+ " vs. "+QString::number(state.OCTOMY_SENDER_ID_SIZE);
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	// REMOTE DESIRED SESSION-ID
	state.readEncDesiredRemoteSessionID();
	if(state.octomyProtocolDesiredRemoteSessionID < FIRST_STATE_ID ) {
		QString es="ERROR: OctoMY Protocol desired remote session ID not valid for SYN-ACK: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	qDebug()<<"SYN-ACK RX REMOTE DESIRED SESSION-ID: "<<state.octomyProtocolDesiredRemoteSessionID;

	// Look up session for remote full ID ( it should already exist, since we should have created it at sendSyn() )
	auto session=lookUpSession(state.octomyProtocolSenderID);
	if(nullptr==session) {
		QString es="ERROR: OctoMY Protocol Session for full ID could not be looked up: " + QString::number(state.octomyProtocolDesiredRemoteSessionID)+ " for SYN-ACK: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	//Verify that we are the initator in this session
	if(!session->handshakeState().isInitiator()) {
		QString es="ERROR: OctoMY Protocol Received syn-ack for session not initiated by us";
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	/*
	//Verify that we are receiving the mode that is expected by the session
	const auto expectedStep=session->handshakeState().nextStep();
	const auto receivedStep=SYN_ACK_OK;
	if(receivedStep!=expectedStep) {
		QString es="ERROR: OctoMY Protocol Received " +handshakeStepToString(expectedStep)+" instead of expected "+handshakeStepToString(receivedStep);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	*/

	// RETURN NONCE
	state.readEncReturnNonce();


	// Verify return nonce
	const SESSION_NONCE_TYPE expectedNonce=session->ourSynNonce();


	qDebug()<< "SYN-ACK RX RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce<<" vs. EXPECTED: "<<expectedNonce;

	if(state.octomyProtocolReturnNonce != expectedNonce) {
		QString es="ERROR: OctoMY Protocol return nonce " + QString::number(state.octomyProtocolReturnNonce)+" did not match expected " +QString::number(expectedNonce)+ " for SYN-ACK";
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	// REMOTE NONCE
	state.readEncRemoteNonce();
	qDebug()<< "SYN-ACK RX REMOTE NONCE WAS: "<< state.octomyProtocolRemoteNonce;

	// Verify remote nonce
	if(state.octomyProtocolRemoteNonce == INVALID_NONCE) {
		QString es="ERROR: OctoMY Protocol remote nonce " + QString::number(state.octomyProtocolRemoteNonce)+" was invalid";
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	// Record remote nonce
	session->setTheirLastNonce(state.octomyProtocolRemoteNonce);
	// Record remote desired session id
	session->setRemoteSessionID(state.octomyProtocolDesiredRemoteSessionID);

	session->handshakeState().setSynAckOK();
	qDebug()<< "SYN-ACK RX handshake state is now :"<< handshakeStepToString(session->handshakeState().step());
}

/*
3. A answers ACK to B:
	Hi again B. HERE IS MY
	  + FULL-ID
	  + RETURN NONCE
	WELL RECEIVED.
*/
void CommsChannel::recieveAck(PacketReadState &state)
{
	// REMOTE FULL-ID
	state.readEncSenderID();
	qDebug()<<"ACK RX FROM "<< state.octomyProtocolSenderID;
	// TODO meta overhead bytes for bytearray from stream
	if(state.octomyProtocolSenderIDRawSize != state.OCTOMY_SENDER_ID_SIZE) { // Full ID should be OCTOMY_FULL_ID_SIZE bytes, no more, no less
		QString es="ERROR: OctoMY Protocol Session-Less Sender ID size mismatch: "+QString::number(state.octomyProtocolSenderIDRawSize)+ " vs. "+QString::number(state.OCTOMY_SENDER_ID_SIZE);
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	// Look up session for remote full ID (INVALID_SESSION_ID means we don't want a new session to be created if it did not already exist, as it should already exist, since we should have created it at receiveSyn() )
	auto session=lookUpSession(state.octomyProtocolSenderID);
	if(nullptr==session) {
		QString es="ERROR: OctoMY Protocol Session for full ID could not be looked up: " + state.octomyProtocolDesiredRemoteSessionID;
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	//Verify that we are not initator in this session
	if(session->handshakeState().isInitiator()) {
		QString es="ERROR: OctoMY Protocol Received ack for session initiated by us";
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	/*
	//Verify that we are receiving the mode that is expected by the session
	const auto expectedStep=session->handshakeState().nextStep();
	const auto receivedStep=ACK_OK;
	if(receivedStep!=expectedStep) {
		QString es="ERROR: OctoMY Protocol Received " +handshakeStepToString(expectedStep)+" instead of expected "+handshakeStepToString(receivedStep);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	*/

	// RETURN NONCE
	state.readEncReturnNonce();
	qDebug()<< "ACK RX RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce;

	// Verify return nonce
	const SESSION_NONCE_TYPE expectedNonce=session->ourAckNonce();
	if(state.octomyProtocolReturnNonce != expectedNonce) {
		QString es="ERROR: OctoMY Protocol return nonce " + QString::number(state.octomyProtocolReturnNonce)+" did not match expected " +QString::number(expectedNonce);
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	session->handshakeState().setAckOK();
	qDebug()<< "ACK RX handshake state is now :"<< handshakeStepToString(session->handshakeState().step());
}



void CommsChannel::recieveData(PacketReadState &state)
{
	qDebug()<<"RX DATA";
	SESSION_ID_TYPE sessionID = (SESSION_ID_TYPE)state.multimagic;
	if(sessionID < FIRST_STATE_ID) {
		QString es="ERROR: invalid multimagic specified";
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	auto session=mSessions.getBySessionID(sessionID);
	if(nullptr==session) {
		//Unknown sender
		QString es="ERROR: OctoMY Protocol Session-Full sender unknown for session-ID: '"+QString::number(sessionID)+QStringLiteral("'. All available are: ")+mSessions.summary();
		qWarning()<<es;
		emit commsError(es);
		return;
	}

	/*
				static const int expectedHeaderSize = sizeof(state.octomyProtocolMagic)+sizeof(state.octomyProtocolFlags)+sizeof(state.octomyProtocolVersion)+sizeof(state.remoteSessionID)
	#ifdef USE_RELIBABILITY_SYSTEM
													  +sizeof(packet_sequence)+sizeof(packet_ack)+sizeof(packet_ack_bits)
	#endif
													  ;//+sizeof(quint32 );
	//qDebug()<<totalRecCount<<"PACKET INITIAL SIZE: "<<size<<", HEADER CALCULATED SIZE: "<<header<<", THUS RAW BYES EXPECTED: "<<(size-header);
				if ( size <= expectedHeaderSize  ) {
					QString es="ERROR: Message too short: " +QString::number(size)+" vs. header: "+QString::number(expectedHeaderSize );
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
				QString es=""+QString::number(partsCount)+" ERROR: OctoMY message type invalid: "+QString::number((quint32)partMessageType,16);
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
					QString es=""+QString::number(partsCount)+" ERROR: Courier read more than was available!";
					qWarning()<<es;
					emit commsError(es);
					return;
				}
				//qDebug()<<"tot Available:"<<totalAvailable<<" available:"<<partBytesAvailable<<" dataStream at end:"<<ds->atEnd();
			} else {
				//TODO: Look at possibility of registering couriers on demand using something like this:
				//emit wakeOnComms(octomy_message_type_int)
				QString es=""+QString::number(partsCount)+" ERROR: No courier found for ID: "+QString::number(state.partMessageTypeID);
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
	qDebug()<<"";
	qDebug()<<"--- RECEIVE to "<< localID()<< " ---";
	mRXRate.countPacket(datagramS.size());
	//countReceived();
	PacketReadState state(datagramS, remoteHostS,remotePortS);

	//QSharedPointer<CommsSession> session;
	state.readMultimagic();
	qDebug()<<"RX Multimagic: "<<MultimagicToString(state.multimagic);
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
		qWarning()<<"ERROR: session was null in doSend";
		return;
	}
	const NetworkAddress na=state.session->address();
	if(!na.isValid(true,false)) {
		qWarning()<<"ERROR: invalid address: "	<<na;
		return;
	}
	if(state.datagram.size()<=0) {
		qWarning()<<"ERROR: datagram is <= 0 bytes ("<<state.datagram.size() <<")";
		return;
	}

	const qint64 written=mUDPSocket.writeDatagram(state.datagram, na.ip(), na.port());
	//qDebug()<<"WROTE "<<written<<" bytes to "<<na.ip()<<":"<<na.port();
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


// Process existing sessions that are in handshake mode
// and create NEW sessions for  id's that have not yet been enrolled
void CommsChannel::sendHandshake(const quint64 &now, const QString handShakeID)
{
	QSharedPointer<CommsSession> session=mSessions.getByFullID(handShakeID);
	if(nullptr==session) {
		session=createSession(handShakeID, true);
	}
	if(nullptr!=session) {
		if(session->established()) {
			QString es="ERROR: Session for handshake with destination "+handShakeID+" was already established";
			qWarning()<<es;
			emit commsError(es);
			return;
		}
		PacketSendState state;
		state.setRemoteID(handShakeID);
		state.session=session;
		HandshakeStep step=session->handshakeState().step();
		//HandshakeStep nextStep=session->handshakeState().nextStep();
		qDebug()<<"STEP "<<handshakeStepToString(step);
		switch(step) {
		default:
				//////////////////// / / / / / //TODO need work on handshake step management
				///
				///
				///
				///
				///     DO IT SOON!
				///
				///
				/// ///////////////////////////////////////////////////////////
		case(VIRGIN): {
			//Verify that we are the initator in this session
			if(!session->handshakeState().isInitiator()) {
				QString es="ERROR: OctoMY Protocol sending syn for session not initiated by us";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			sendSyn(state);
		}
		break;
		case(SYN_OK): {
			//Verify that we are not initator in this session
			if(session->handshakeState().isInitiator()) {
				QString es="ERROR: OctoMY Protocol sending syn-ack for session initiated by us";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			sendSynAck(state);
		}
		break;
		case(SYN_ACK_OK): {
			//Verify that we are the initator in this session
			if(!session->handshakeState().isInitiator()) {
				QString es="ERROR: OctoMY Protocol sending ack for session not initiated by us";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			sendAck(state);
		}
		break;
		case(ACK_OK): {

		}
		break;
		}
	} else {
		QString es="ERROR: OctoMY Protocol session could not be created for: " + handShakeID;
		qWarning()<<es;
		emit commsError(es);
		return;
	}
}



////////////////////////////////////////////////////////////////////////////////



/*
1. A sends SYN to B:
	Hi B. Here is my
	  + FULL-ID
	  + DESIRED SESSION-ID
	  + NONCE
	ENCODED WITH YOUR(B's) PUBKEY.
*/
void CommsChannel::sendSyn(PacketSendState &state)
{
	// Send
	state.writeMultimagic(MULTIMAGIC_SYN);
	// FULL-ID
	state.writeEncSenderID(localID());
	// DESIRED SESSION-ID
	const SESSION_ID_TYPE localSessionID=state.session->localSessionID();
	qDebug()<<"SYN TX LOCAL SESSION ID="<< localSessionID;
	state.writeEncSessionID(localSessionID);
	// NONCE
	const SESSION_NONCE_TYPE synNonce=state.session->createOurSynNonce();
	qDebug()<<"SYN TX NONCE CREATED="<<synNonce;
	state.writeEncNonce(synNonce);
	state.encrypt();
	state.writeProtocolEncryptedMessage();
	doSend(state);

	state.session->handshakeState().setSynOK();
	qDebug()<< "SYN TX handshake state is now :"<< handshakeStepToString(state.session->handshakeState().step());
}


////////////////////////////////////////////////////////////////////////////////


/*
2. B answers SYN-ACK to A:
	Hi A. HERE IS MY
	  + FULL-ID
	  + DESIRED SESSION-ID
	  + RETURN NONCE
	  + NEW NONCE
	ENCODED WITH YOUR(A's) PUBKEY back atch'a.
*/
void CommsChannel::sendSynAck(PacketSendState &state)
{
	// Send
	state.writeMultimagic(MULTIMAGIC_SYNACK);
	// FULL-ID
	state.writeEncSenderID(localID());
	// DESIRED SESSION-ID
	const SESSION_ID_TYPE localSessionID=state.session->localSessionID();
	qDebug()<<"SYN-ACK DESIRED SESSION-ID: "<<localSessionID;
	state.writeEncSessionID(localSessionID);
	const SESSION_NONCE_TYPE theirNonce=state.session->theirLastNonce();
	qDebug()<<"SYN-ACK TX NONCE RETURNED: "<<theirNonce;
	state.writeEncNonce(theirNonce);
	const SESSION_NONCE_TYPE ackNonce=state.session->createOurAckNonce();
	qDebug()<<"SYN-ACK TX NONCE CREATED: "<<ackNonce;
	state.writeEncNonce(ackNonce);
	state.encrypt();
	state.writeProtocolEncryptedMessage();
	doSend(state);
	state.session->handshakeState().setSynAckOK();
	qDebug()<< "SYN-ACK TX handshake state is now :"<< handshakeStepToString(state.session->handshakeState().step());
}

////////////////////////////////////////////////////////////////////////////////

/*

3. A answers ACK to B:
	Hi again B. HERE IS MY
	  + FULL-ID
	  + RETURN NONCE
	WELL RECEIVED.
*/
void CommsChannel::sendAck(PacketSendState &state)
{
	// Send
	state.writeMultimagic(MULTIMAGIC_ACK);
	// FULL-ID
	state.writeEncSenderID(localID());
	// RETURN NONCE
	const SESSION_NONCE_TYPE theirNonce=state.session->theirLastNonce();
	qDebug()<<"ACK NONCE RETURNED: "<<theirNonce;
	state.writeEncNonce(theirNonce);
	state.encrypt();
	state.writeProtocolEncryptedMessage();
	doSend(state);
	state.session->handshakeState().setAckOK();
	qDebug()<< "ACK TX handshake state is now :"<< handshakeStepToString(state.session->handshakeState().step());
}


////////////////////////////////////////////////////////////////////////////////



void CommsChannel::sendIdle(const quint64 &now, QSharedPointer<CommsSession> session)
{
	// TODO: See if all this session stuff is necessary
	/*
	if(nullptr==session) {
		qWarning()<<"ERROR: session was null";
		return;
	}
	*/
	PacketSendState state;

	state.writeMultimagic(MULTIMAGIC_IDLE);
	/*
	state.setSession(session);

	auto sessionID=session->remoteSessionID();
	// TODO: What tha hall
	state.writeSessionID(sessionID);
	*/
	state.encrypt();
	state.writeProtocolEncryptedMessage();


	doSend(state);
}



////////////////////////////////////////////////////////////////////////////////

void CommsChannel::sendCourierData(const quint64 &now, Courier &courier)
{
	QString toID=courier.destination();
	if(toID.isEmpty()) {
		QString es="ERROR: Invalid courier destination when sending data";
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	QSharedPointer<CommsSession> session=lookUpSession(toID);
	if(nullptr==session) {
		QString es="ERROR: Could not find session for courier with destination "+toID;
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	if(!session->established()) {
		QString es="ERROR: Session for courier with destination "+toID+" was not fully established";
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	PacketSendState state;
	state.setRemoteID(toID);
	state.session=session;
	// Ladies, the bar is open!
	qDebug()<<"LADIES, THE BAR IS OPEN FOR " << toID;
	/*
			const qint32 availableBytes = MAX_UDP_PAYLOAD_SIZE - ( 7 * 4 );

			state.writeMagic();
			state.writeProtocolVersion();

			auto sessionID=state.session->remoteSessionID();
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
	*/

}




quint64 CommsChannel::rescheduleSending(quint64 now)
{
	mTXScheduleRate.countPacket(0);
	//qDebug()<<" ... R E S C H E D U L E ... "<<now;
	mMostUrgentSendingTime=MINIMAL_PACKET_RATE;
	mSchedule.clear();
	// Update first
	for(Courier *courier:mCouriers) {
		if(nullptr!=courier) {
			courier->update(now);
		}
	}
	mPendingHandshakes.clear();
	// Re-schedule couriers (strive to adhere to the mandates set fourth by each courier)
	for(Courier *courier:mCouriers) {
		if(nullptr!=courier) {
			CourierMandate cm=courier->mandate();
			//qDebug()<<"MANDATE FOR "<<courier->name()<<" IS "<<cm.toString();
			if(cm.sendActive) {
				// See if the node for this courier has been enrolled yet
				const QString id=courier->destination();
				QSharedPointer<CommsSession> session=mSessions.getByFullID(id);
				if(nullptr!=session) {
					if(session->established()) {
						//const QString &courierDestination=courier->destination();
						const qint64 timeLeft = cm.nextSend - now;
						// Update most-urgent time
						if(timeLeft < mMostUrgentSendingTime) {
							const quint64 lastUrgent=mMostUrgentSendingTime;
							mMostUrgentSendingTime=qMax(timeLeft, 0LL);
							qDebug()<<courier->name()<<courier->id()<<courier->ser()<<" WITH TIME LEFT " << timeLeft<< " BUMPED URGENT FROM "<<lastUrgent<<" -> "<<mMostUrgentSendingTime;
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
					} else {
						mPendingHandshakes << id;
					}
				} else {
					mPendingHandshakes << id;
				}
			}
		}
	}
	// Ensure we get immediate sending opportunities for sessions that are in handshake
	if(mSessions.hasHandshakers()) {
		mMostUrgentSendingTime=qMin(mMostUrgentSendingTime,0LL);
	}
	// Prepare for next sending opportunity

	const quint64 delay=qBound((qint64)MAXIMAL_PACKET_RATE, mMostUrgentSendingTime, (qint64)MINIMAL_PACKET_RATE);
	qDebug()<<"NEW SCHEDULE DELAY: "<<delay<<" ("<<mMostUrgentSendingTime<<")";
	mSendingTimer.setInterval(delay);
	return delay;
}




//////////////////////////////////////////////////
// Send & receive slots

void CommsChannel::onSendingTimer()
{
	qDebug()<<"";
	qDebug()<<"--- SEND from "<< localID()<< " ---";
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	detectConnectionChanges(now);
	mTXOpportunityRate.countPacket(0);
	// First look at handshakes
	qDebug()<<"HANDSHAKES: "<<mPendingHandshakes.size();
	for(QSet<QString> ::const_iterator i=mPendingHandshakes.begin(); i!= mPendingHandshakes.end(); ) {
		const QString handShakeID=(*i);
		qDebug() << " + " << handShakeID;
		sendHandshake(now, handShakeID);
		// Increase counter here to ensure that any removed items don't crash our loop
		++i;
	}
	// Next look at courier schedule
	qDebug()<<"SCHEDULE: "<<mSchedule.size();
	for (QMap<quint64, Courier *>::iterator i = mSchedule.begin(); i != mSchedule.end(); ) {
		Courier *courier=i.value();
		qDebug() << " + " << i.key() << " = " << ((nullptr==courier)?"NULL":courier->toString());
		// Increase counter here to ensure that any removed items don't crash our loop
		++i;
	}
	// ... and send data according to schedule
	for (QMap<quint64, Courier *>::iterator i = mSchedule.begin(); i != mSchedule.end(); ) {
		Courier *courier=i.value();
		if(nullptr!=courier) {
			sendCourierData(now, *courier);
		} else {
			qWarning()<<"WARNING: scheduled courier was null";
		}
		// Increase counter here to ensure that any removed items don't crash our loop
		++i;
	}
	// Finally send idle packets to sessions that need it
	auto idle=mSessions.getByIdleTime(MINIMAL_PACKET_RATE);
	const int isz=idle.size();
	qDebug()<<"IDLES: "<< isz;
	if(isz>0) {
		for(QSharedPointer<CommsSession> session:idle) {
			if(nullptr!=session) {
				sendIdle(now, session);
			} else {
				qWarning()<<"ERROR: session was null when sending idle packet";
			}
		}
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
	auto key=mKeystore.localKey();
	if(nullptr==key) {
		return "";
	}
	return key->id();
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
	qDebug()<<"----- READY READ for "<<localID();
	while (mUDPSocket.hasPendingDatagrams()) {
		qDebug()<<"      + UDP PACKET";
		QByteArray datagram;
		datagram.resize(mUDPSocket.pendingDatagramSize());
		QHostAddress host;
		quint16 port=0;
		qint64 ret=mUDPSocket.readDatagram(datagram.data(), datagram.size(), &host, &port);
		if(ret>=0) {
			receivePacketRaw(datagram,host,port);
		} else {
			qWarning()<<"ERROR: error reading pending datagram";
		}
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
