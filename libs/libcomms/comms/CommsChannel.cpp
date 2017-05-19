/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "CommsChannel.hpp"
#include "CommsSessionDirectory.hpp"

#include "utility/Standard.hpp"
#include "comms/CommsSession.hpp"
#include "messages/MessageType.hpp"

#include "security/KeyStore.hpp"

#include "pose/Pose.hpp"


#include <QDataStream>
#include <QDateTime>

quint32 CommsChannel::sTotalRecCount=0;

// Special purpose Session ID  that signals from remote end that they want to start a new session
#define NEW_SESSION_ID (0)

CommsChannel::CommsChannel(const QString &id, KeyStore &keystore, QObject *parent)
	: QObject(parent)
	, mKeystore(keystore)
	, mSessions(mKeystore)
	, mLocalID(id)
	, mLocalSessionID(0)
	  //, mLocalSignature(id)
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
	//mLocalSignature.setAddress(localAddress);
	bool b = mUDPSocket.bind(mLocalAddress.ip(), mLocalAddress.port());
	qDebug()<<"----- comms bind "<< mLocalAddress.toString()<< mLocalID<< (b?" succeeded": " failed");
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
	qDebug()<<"----- comms unbind "<< mLocalAddress.toString() << mLocalID;
}


/*
CommsSignature CommsChannel::localSignature()
{
	return mLocalSignature;
}
*/

void CommsChannel::countReceived()
{
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	mLastRX=now;
	sTotalRecCount++;
	mRxCount++;
	//qDebug()<<totalRecCount<<"--- RECEIV";
	auto timeSinceLast=mLastRX-mLastRXST;
	if(timeSinceLast>1000) {
		qDebug().noquote()<<"Packet Receive Count="<<QString::number(mRxCount/(timeSinceLast/1000))<<"/sec";
		mLastRXST=mLastRX;
		mRxCount=0;
	}
}

struct PacketReadState {

public:
	QByteArray datagram;
	QHostAddress remoteHost;
	quint16 remotePort;
	QSharedPointer<QDataStream> stream;

	quint64 remoteSessionID;
	const int size;
	int totalAvailable;

	quint32 octomyProtocolMagic;
	quint8 octomyProtocolFlags;
	quint32 octomyProtocolVersion;

	QByteArray octomyProtocolEncryptedMessage;
	int octomyProtocolEncryptedMessageSize;




	quint32 partMessageTypeID;
	quint16 partBytesAvailable;


public:
	PacketReadState(QByteArray datagram, QHostAddress remoteHost , quint16 remotePort)
		: datagram(datagram)
		, remoteHost(remoteHost)
		, remotePort(remotePort)
		, stream(new QDataStream(&this->datagram, QIODevice::ReadOnly))
		, remoteSessionID(NEW_SESSION_ID)
		, size(datagram.size())
		, totalAvailable(size)
		, octomyProtocolMagic(0)
		, octomyProtocolFlags(0)
		, octomyProtocolVersion(0)
		, octomyProtocolEncryptedMessageSize(0)
		, partMessageTypeID(0)
		, partBytesAvailable(0)
	{

	}

	// Read session ID
	void readSessionID()
	{

		*stream >> remoteSessionID;
		totalAvailable-=sizeof(remoteSessionID);
	}

	// Read protocol MAGIC
	void readProtocolMagic()
	{
		*stream >> octomyProtocolMagic;
		totalAvailable-=sizeof(octomyProtocolMagic);

	}

	// Read protocol FLAGS
	void readProtocolFlags()
	{
		*stream >> octomyProtocolFlags;
		totalAvailable-=sizeof(octomyProtocolFlags);

	}

	// Read protocol VERSION
	void readProtocolVersion()
	{
		*stream >> octomyProtocolVersion;
		totalAvailable-=sizeof(octomyProtocolVersion);
	}

	static const int OCTOMY_ENCRYPTED_MESSAGE_SIZE=20;

	// Read Pub-key encrypted message body
	void readProtocolEncryptedMessage()
	{
		*stream >> octomyProtocolEncryptedMessage;
		octomyProtocolEncryptedMessageSize=octomyProtocolEncryptedMessage.size();
		totalAvailable-=octomyProtocolEncryptedMessageSize;
	}

	void readPartMessageTypeID()
	{
		partMessageTypeID=0;
		*stream >> partMessageTypeID;
		totalAvailable-=sizeof(quint32);
	}

	void readPartBytesAvailable()
	{
		partBytesAvailable=0;
		*stream >> partBytesAvailable;
		totalAvailable-=sizeof(quint16);
	}

};




/*
	 + A packet is defined as initial when it contains a session ID of 0
	 + A packet is defined as broken when it does not adhere to the protocol by displaying a lack/excess of data or data in the wrong format
	 + A packet is defined as whole when it is not broken
	 + A packet is defined as hacked when it is whole according to comms protocol but broken according to tamper protocol

	Comms protocol is the language of OctoMY™ in network communication.
	Tamper protocol is an extra layer of protection beside comms protocol to detect attempts to tamper with communications. It has validation checks that are not necessary by the comms protocol, but add tells to the authenticity of the data (would the real implementation send data in this way)?

	A valid handshake to start a new session looks like this (similar in concept to the 3-way TCP handshake):

	1. A sends SYN to B:
		Hi B. Here is my FULL-ID + NONCE ENCODED WITH YOUR PUBKEY.

	2. B answers SYN-ACK to A:
		Hi A. HERE IS MY FULL-ID + SAME NONCE + NEW NONCE ENCODED WITH YOUR PUBKEY back atch'a.

	3. A answers ACK to B:
		Hi again B. FULL-ID + NONCES WELL RECEIVED.

	At this point session is established

	NOTE: For every step in this protocol, if A is waiting for data from B that does not arrive, it will attempt to resend it's last message on a regular interval until it does (or some error condition or other state change occurs).

	Comms have two "layers", the intrinsic layer and the courier layer.
	The courier part is reserved for the application layer that wish to conduct communication using comms channel.
	The intrinsic part is reserved for internal affairs of commschannel.
	Intrinsic and courier parts of commschannel should not depend directly on one another and their respective implementation should not be mixed.

	Intrinsic parts of comms include the following:

	 + Session management
		 + Session initiation/handshake
		 + Exchange of transmission control data
		 + Session tear-down
	 + Bandwidth management
		 + Detection of available bandwidth
		 + Throttling to avoid excessive bandwidth use
		 + Continuous monitoring and ajustment of protocol parameters optimize flow of packets (including the priority and timing of couriers)
	 + Encryption management
		 + Signing and sign verification of un-encrypted protocol text based on client RSA keypairs
		 + Generation and exchange of encryption keys based on client RSA keypairs
		 + Generation of security primitives such as nonces
		 + Encryption and decryption of protocol text
	 + Reliability management
		 + Maintaining of continued UDP connection over unreliable network components such as consumer grade routers and wireles radios with poor coverage by the dispatch of necessary communication with STUN services, or sending of antler packets.
		 + Detection and removal of duplicate and corrupt packets
		 + Detection and re-sending of missing packets
		 + Reordering of ordered packet sequences

	Please note that the the expensive and complex intrinsic features such as
	relibability and encryption of CommsChannel are invoked only when needed.

	When the amount of data needed for intrinsic features is extensive, separate
	"intrinsic packets" will be sent, while other lesser-in-size intrinsic data
	such as counters will instead accompany each packet. Protocol dictates when
	such dedicated packets will be needed or not, and changes in this part of the
	protocol should not affect the higher level courier interface.
*/

/* Flags:
	+ Bit 1	"SYN"
	+ Bit 2 "ACK"
	+ Bit 3
	+ Bit 4 "Reserved for future use"
	+ Bit 5 "Reserved for future use"
	+ Bit 6 "Reserved for future use"
	+ Bit 7 "Reserved for future use"
	+ Bit 8 "Reserved for future use"
*/


const quint32 OCTOMY_PROTOCOL_FLAG_ACK=(1<<0);
const quint32 OCTOMY_PROTOCOL_FLAG_SYN=(1<<1);

// Main packet reception
void CommsChannel::receivePacketRaw( QByteArray datagramS, QHostAddress remoteHostS , quint16 remotePortS )
{
	countReceived();
	PacketReadState *state=new PacketReadState(datagramS, remoteHostS,remotePortS);
	if(nullptr==state) {
		QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol could not create state";
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	state->readSessionID();
	QSharedPointer<CommsSession> session;
	// Sender claims this packet is an initial packet for the establishment of a session
	if(NEW_SESSION_ID==state->remoteSessionID) {

		// All sync packets are high on robustness, which means magic + version + encrypted body is sent with all of them

		state->readProtocolMagic();
		if(OCTOMY_PROTOCOL_MAGIC!=state->octomyProtocolMagic) {
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Magic mismatch: "+QString::number(state->octomyProtocolMagic,16)+ " vs. "+QString::number((quint32)OCTOMY_PROTOCOL_MAGIC,16);
			qWarning()<<es;
			emit commsError(es); //TODO: Handle this by making a few retries before requiering user to aprove further retries (to avoid endless hammering)
			return;
		}

		state->readProtocolVersion();
		switch(state->octomyProtocolVersion) {
		case(OCTOMY_PROTOCOL_VERSION_CURRENT): {
			state->stream->setVersion(OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT);
		}
		break;
		// TODO: Look at good ways to stay backward compatible (long term goal, right now this is not feasible due to the frequent changes to the protocol)
		default: {
			QString es=QString::number(sTotalRecCount)+"ERROR: OctoMY Protocol version unsupported: "+QString::number(state->octomyProtocolVersion);
			qWarning()<<es;
			emit commsError(es);
			return;
		}
		break;
		}

		// First data of sync packet is flags, which dictate what the rest of the data will be
		state->readProtocolFlags();


		// Read Pub-key encrypted message body
		state->readProtocolEncryptedMessage();
		if(state->octomyProtocolEncryptedMessageSize != state->OCTOMY_ENCRYPTED_MESSAGE_SIZE) { // Encrypted message should be OCTOMY_ENCRYPTED_MESSAGE_SIZE bytes, no more, no less
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less encrypted message size mismatch: "+QString::number(state->octomyProtocolEncryptedMessageSize)+ " vs. "+QString::number(state->OCTOMY_ENCRYPTED_MESSAGE_SIZE);
			qWarning()<<es;
			emit commsError(es);
			return;
		}

		// Decrypt message body using local private-key
		Key &localKey=mKeystore.localKey();
		QByteArray octomyProtocolDecryptedMessage=localKey.decrypt(state->octomyProtocolEncryptedMessage);
		const int octomyProtocolDecryptedMessageSize=octomyProtocolDecryptedMessage.size();
		if(0 == octomyProtocolDecryptedMessageSize) { // Size of decrypted message should be non-zero
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less nonce decryption failed";
			qWarning()<<es;
			emit commsError(es);
			return;
		}

		QDataStream decryptedStream(&octomyProtocolDecryptedMessage, QIODevice::ReadOnly);

		// Extract full ID of sender
		static const int OCTOMY_SENDER_ID_SIZE=20;
		QByteArray octomyProtocolSenderIDRaw;   //octomyProtocolDecryptedMessage.mid(offset, OCTOMY_SENDER_ID_SIZE);
		decryptedStream >> octomyProtocolSenderIDRaw;
		const int octomyProtocolSenderIDRawSize=octomyProtocolSenderIDRaw.size();
		if(octomyProtocolSenderIDRawSize != OCTOMY_SENDER_ID_SIZE) { // Full ID should be OCTOMY_FULL_ID_SIZE bytes, no more, no less
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less Sender ID size mismatch: "+QString::number(octomyProtocolSenderIDRawSize)+ " vs. "+QString::number(OCTOMY_SENDER_ID_SIZE);
			qWarning()<<es;
			emit commsError(es);
			return;
		}
		const QString octomyProtocolSenderID=octomyProtocolSenderIDRaw.toHex();
		qDebug()<< "SESSION-LESS PACKET RX FROM "<< octomyProtocolSenderID;

		// Look for existing sessions tied to this ID
		session=mSessions.getByFullID(octomyProtocolSenderID);
		if(nullptr==session) {
			if(mKeystore.hasPubKeyForID(octomyProtocolSenderID)) {
				Key key=mKeystore.pubKeyForID(octomyProtocolSenderID);
				if(key.isValid(true)) {
					session=QSharedPointer<CommsSession>(new CommsSession(octomyProtocolSenderID, key));
					mSessions.insert(session);
					qDebug()<< "NEW SESSION CREATED FOR ID "<<octomyProtocolSenderID;
				} else {
					QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Could not create session for sender  with ID "+octomyProtocolSenderID;
					qWarning()<<es;
					emit commsError(es);
					return;
				}
			} else {
				//Unknown sender
				QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Less sender unknown";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
		} else {
			qDebug()<<"SESSIONLESS CORRESPONDANT: "<<session->address().toString()<<QStringLiteral(", ")<<session->key().id();
		}
		// Did handshake complete already?
		if(session->established()) {
			// TODO: Handle this case:
			/*
							When an initial packet is received after session was already established the following logic applies:
								+ If packet timestamp was before session-established-and-confirmed timestamp, it is ignored
								+ The packet is logged and the stream is examined for packets indicating the original session is still in effect.
								+ If there were no sign of the initial session after a timeout of X seconds, the session is torn down, and the last of the session initial packet is answered to start a new hand shake
								+ If one or more packets indicating that the session is still going, the initial packet and it's recepient is flagged as hacked and the packet is ignored. This will trigger a warning to the user in UI, and rules may be set up in plan to automatically shut down communication uppon such an event.
								+ No matter if bandwidth management is in effect or not, valid initial packets should be processed at a rate at most once per 3 seconds.
				*/
		} else {
			switch(state->octomyProtocolFlags & (OCTOMY_PROTOCOL_FLAG_ACK | OCTOMY_PROTOCOL_FLAG_SYN)) {
			//1. A sends SYN to B: Hi B. Here is my ( FULL-ID + NONCE + A's DESIRED SESSION-ID ) ENCODED WITH YOUR PUBKEY.
			case(OCTOMY_PROTOCOL_FLAG_SYN): {

				// Extract remote nonce
				quint32 octomyProtocolRemoteNonce=0;
				decryptedStream >> octomyProtocolRemoteNonce;

				// Extract desired remote session ID
				quint16 octomyProtocolDesiredRemoteSessionID=0;
				decryptedStream >> octomyProtocolDesiredRemoteSessionID;

			}
			break;
			// 2. B answers SYN-ACK to A: Hi A. Here is my ( FULL-ID + NEW NONCE + RETURN NONCE + B's DESIRED SESSION-ID ) ENCODED WITH YOUR PUBKEY back atch'a.
			case(OCTOMY_PROTOCOL_FLAG_ACK | OCTOMY_PROTOCOL_FLAG_SYN): {

				// Extract remote nonce
				quint32 octomyProtocolRemoteNonce=0;
				decryptedStream >> octomyProtocolRemoteNonce;

				// Extract return nonce
				quint32 octomyProtocolReturnNonce=0;
				decryptedStream >> octomyProtocolReturnNonce;

				// Extract desired remote session ID
				quint16 octomyProtocolDesiredRemoteSessionID=0;
				decryptedStream >> octomyProtocolDesiredRemoteSessionID;

			}
			break;
			// 3. A answers ACK to B: Hi again B. Here is my ( FULL-ID + RETURN NONCES ) ENCODED WITH YOUR PUBKEY back atch'a.
			case(OCTOMY_PROTOCOL_FLAG_ACK): {
				// Extract return nonce
				quint32 octomyProtocolReturnNonce=0;
				decryptedStream >> octomyProtocolReturnNonce;

			}
			break;
			default:
			case(0): {

			} break;
			}

			//const bool ack = (0!=(state->octomyProtocolFlags & OCTOMY_PROTOCOL_FLAG_ACK));		const bool syn = (0!=(state->octomyProtocolFlags & OCTOMY_PROTOCOL_FLAG_SYN));

		}

	}
	// Sender claims this packet is for a previously established session
	else {
		session=mSessions.getBySessionID(state->remoteSessionID);
		if(nullptr==session) {
			//Unknown sender
			QString es=QString::number(sTotalRecCount)+" ERROR: OctoMY Protocol Session-Full sender unknown for session-ID: "+state->remoteSessionID;
			qWarning()<<es;
			emit commsError(es);
			return;
		}

		qDebug()<<"SESSIONFULL CORRESPONDANT: "<<session->address().toString()<<", "<<session->key().id()<< QStringLiteral(" sid:")<< (state->remoteSessionID);

		/*
					static const int expectedHeaderSize = sizeof(state->octomyProtocolMagic)+sizeof(state->octomyProtocolFlags)+sizeof(state->octomyProtocolVersion)+sizeof(state->remoteSessionID)
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

		if(nullptr==session) {
			QString es=QString::number(sTotalRecCount)+"ERROR: Could not fetch client by id: '"+state->remoteSessionID+"'";
			qWarning()<<es;
			emit commsError(es);
			return;
		}

		session->receive();
		quint16 partsCount=0;
		const qint32 minAvailableForPart  = ( sizeof(quint32)  );
		if(state->totalAvailable < minAvailableForPart) {
			qWarning()<<"ERROR: NO PARTS DETECTED! Data size too small";
		}
		while(state->totalAvailable >= minAvailableForPart) {
			partsCount++;
			//qDebug()<<"READING PART #"<<partsCount<<" WITH "<<totalAvailable<<" vs. "<<minAvailableForPart;
			state->readPartMessageTypeID();
			state->readPartBytesAvailable();
			// Is this an internal part?
			if(state->partMessageTypeID < Courier::FIRST_USER_ID) {
				//Use message type enum for built in messages
				const MessageType partMessageType=(MessageType)state->partMessageTypeID;
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
				Courier *courier=getCourierByID(state->partMessageTypeID);
				if(nullptr!=courier) {
					const quint16 bytesSpent=courier->dataReceived(*state->stream, state->partBytesAvailable);
					const int left=state->partBytesAvailable-bytesSpent;
					//qDebug()<<totalRecCount<<"GOT COURIER MSG "<<partMessageTypeID<<" "<<c->name()<<" bytes avail="<<partBytesAvailable<<" tot avail="<<totalAvailable<<" bytes spent="<<bytesSpent<<" left="<<left<<"  ";
					state->totalAvailable-=bytesSpent;
					if(left>=0) {
						if(left>0) {
							qWarning()<<sTotalRecCount<<"WARNING: SKIPPING "<<left<<" LEFTOVER BYTES AFTER COURIER WAS DONE";
							state->stream->skipRawData(left);
							state->totalAvailable-=left;
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
					QString es=QString::number(sTotalRecCount)+" "+QString::number(partsCount)+" ERROR: No courier found for ID: "+QString::number(state->partMessageTypeID);
					qWarning().noquote()<<es;
					emit commsError(es);
					return;
				}
				//qDebug()<<totalRecCount<<partsCount<<"PART DONE";
			}
		}
		if(state->totalAvailable > 0 ) {
			QString es="WARNING: Not all bytes in datagram were read/used. There were "+QString::number(state->totalAvailable)+" bytes left after reading "+QString::number(partsCount)+" parts";
			qWarning()<<es;
			return;
		}

	}
}


void CommsChannel::sendData(const quint64 &now, QSharedPointer<CommsSession> session, Courier *courier)
{
	QString toID=(nullptr!=courier)?(courier->destination()):session->fullID();
	if(toID.isEmpty()) {
		qWarning()<<"ERROR: invalid ID when sending data";
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




	quint64 sessionID=session->sessionID();

	ds << sessionID;
	bytesUsed += sizeof(sessionID);

#ifdef USE_RELIBABILITY_SYSTEM
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
#endif
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
		auto na=mLocalAddress;
		const qint64 written=mUDPSocket.writeDatagram(datagram, na.ip(), na.port());
		//qDebug()<<"WROTE "<<written<<" bytes to "<<sig;
		if(written<sz) {
			qDebug()<<"ERROR: Only " << written << " of " <<sz<<" written to UDP SOCKET:"<<mUDPSocket.errorString()<< " for destination "<< mLocalAddress.toString()<<mLocalID;
			return;
		} else {
			qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
		}
		session->countSend(written);
	}
}


quint64 CommsChannel::rescheduleSending(quint64 now)
{
	qDebug()<<" ... R E S C H E D U L E ... "<<now;
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
			//qDebug()<<"MANDATE FOR "<<courier->name()<<" IS "<<cm.toString();
			if(cm.sendActive) {
				const QString &courierDestination=courier->destination();
				//const quint64 last = courier->lastOpportunity();
				const qint64 timeLeft = cm.nextSend - now;
				if(timeLeft < mMostUrgentCourier) {
					const quint64 lastUrgen=mMostUrgentCourier;
					mMostUrgentCourier=timeLeft;
					qDebug()<<courier->name()<<courier->id()<<" WITH TIME LEFT " << timeLeft<< " BUMPED URGENT FROM "<<lastUrgen<<" TO "<<mMostUrgentCourier;
				}
				//We are (over)due, send a packet!
				if(timeLeft < 0) {
					const quint64 score=(cm.priority * -timeLeft );
					mPri.insert(score, courier); //TODO: make this broadcast somehow (use ClientDirectory::getByLastActive() and ClientSignature::isValid() in combination or similar).
					qDebug()<<courier->name()<<courier->id()<<" SCHEDULED FOR SEND WITH TIMELEFT "<<timeLeft<<" AND SCORE "<<score;
				} else if(now-courier->lastOpportunity() > MINIMAL_PACKET_RATE) {
					mIdle.push_back(&courierDestination);
					qDebug()<<courier->name()<<courier->id()<<" SCHEDULED FOR IDLE";
				}


			}
		}
	}
	// Prepare for next round (this implies a stop() )
	const quint64 delay=qBound((qint64) 0, mMostUrgentCourier, (qint64)MINIMAL_PACKET_RATE);
	qDebug()<<"NEW SCHEDULE DELAY: "<<delay<<" ("<<mMostUrgentCourier<<")";
	mSendingTimer.start(delay);
	return delay;
}


void CommsChannel::onSendingTimer()
{
	qDebug()<<"--- SEND";
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 timeSinceLastRX=now-mLastRX;

	if(mConnected && timeSinceLastRX>CONNECTION_TIMEOUT) {
		qDebug()<<"Connection timed out, stopping";
		stop();
	} else if(!mConnected && timeSinceLastRX<=CONNECTION_TIMEOUT) {
		mConnected=true;
		emit commsConnectionStatusChanged(true);
	}
	mLastRX=now;
	mTxCount++;
	if(now-mLastTXST>1000) {
		qDebug()<<"SEND OPPORUNITY count="<<QString::number(mTxCount)<<"/sec";
		mLastTXST=now;
		mTxCount=0;
	}
	// Write one message per courier in pri list
	// NOTE: several possible optimizations exist, for example grouping messages
	//       to same node in one packet etc. We may exploit them in the future.
	for (QMap<quint64, Courier *>::iterator i = mPri.begin(), e=mPri.end(); i != e; ++i) {
		Courier *courier=i.value();
		if(nullptr!=courier) {
			QString id=courier->destination();
			QSharedPointer<CommsSession> session=mSessions.getByFullID(id);
			if(nullptr==session) {
				qWarning()<<"ERROR: No session found for courier "<<courier->name()<<" with destination "<<id;
				return;
			}
			sendData(now, session, courier);
		}
	}
	const int isz=mIdle.size();
	if(isz>0) {
		qDebug()<<"Send "<<isz<<" idle packets";
		for(const QString *id:mIdle) {
			if(nullptr!=id) {
				QSharedPointer<CommsSession> session=mSessions.getByFullID(*id);
				if(nullptr==session) {
					qWarning()<<"ERROR: No session found for idle with destination "<<*id;
					return;
				}
				sendData(now, session, nullptr);
			}
		}
	}
	rescheduleSending(now);
}

//Only for testing purposes! Real data should pass through the courier system
//and be dispatched by logic in sending timer
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
	return mLocalID;
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


void CommsChannel::setID(const QString &id)
{
	mLocalID=id;

}

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
