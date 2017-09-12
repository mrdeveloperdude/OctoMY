/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "CommsChannel.hpp"

#include "CommsCarrier.hpp"

#include "CommsSessionDirectory.hpp"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "comms/CommsSession.hpp"
#include "messages/MessageType.hpp"

#include "security/KeyStore.hpp"

#include "pose/Pose.hpp"

#include "discovery/NodeAssociateStore.hpp"

#include "PacketSendState.hpp"
#include "PacketReadState.hpp"

#include "IDDuel.hpp"

#include <QDataStream>
#include <QDateTime>

#define FIRST_STATE_ID ((SESSION_ID_TYPE)MULTIMAGIC_LAST)


//#define DO_CC_ENC

CommsChannel::CommsChannel(CommsCarrier &carrier, KeyStore &keystore, NodeAssociateStore &peers, QObject *parent)
	: QObject(parent)
	, mCarrier(carrier)
	, mKeystore(keystore)
	, mPeers(peers)
	, mSessions(mKeystore)
	, mLocalSessionID(0)
	, mTXScheduleRate("CC TX SCHED")
//	, mConnected(false)
{
	setObjectName("CommsChannel");
	mCarrier.setHookCarrierSignals(*this, true);
}

/*
//TODO: Remove once nobody refers to it any more
CommsChannel::CommsChannel(CommsCarrier &carrier, KeyStore &keystore, QObject *parent)
	: QObject(parent)
	, mCarrier(carrier)
	, mKeystore(keystore)
	, mPeers(*((NodeAssociateStore *) nullptr ))//HACK
	, mSessions(mKeystore)
{
	qWarning()<<"ERROR: PLEASE USE THE OTHER CONSTRUCTOR AS THIS IS UNSUPPORTEDF!";
}
*/

CommsChannel::~CommsChannel()
{
	mCarrier.setHookCarrierSignals(*this, false);
}

CommsSessionDirectory &CommsChannel::sessions()
{
	return mSessions;
}

void CommsChannel::start(NetworkAddress localAddress)
{
	mCarrier.start(localAddress);
}



void CommsChannel::stop()
{
	mCarrier.stop();
}

bool CommsChannel::isStarted() const
{
	return mCarrier.isStarted();
}

bool CommsChannel::isConnected() const
{
	return mCarrier.isConnected();
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

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
		// NOTE: Even if we don't actually decrypt, we must still call this!
		state.decrypt(*localKey);
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
		QString es="ERROR: OctoMY Protocol version unsupported: "+QString::number(state.octomyProtocolVersion);
		qWarning()<<es;
		emit commsError(es);
		return false;
	}
	break;
	}
	return true;
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
						session=QSharedPointer<CommsSession>(OC_NEW CommsSession(key));
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


void CommsChannel::recieveHandshake(PacketReadState &state)
{
	if(recieveEncryptedBody(state)) {

		const auto multimagic=(Multimagic)state.multimagic;
		const bool remoteWantsToBeInitiator=( ( MULTIMAGIC_SYN == multimagic ) ||  ( MULTIMAGIC_ACK == multimagic )  );


		// REMOTE FULL-ID
		state.readEncSenderID();
		// TODO meta overhead bytes for bytearray from stream
		if(state.octomyProtocolSenderIDRawSize != state.OCTOMY_SENDER_ID_SIZE) { // Full ID should be OCTOMY_FULL_ID_SIZE bytes, no more, no less
			QString es="ERROR: OctoMY Protocol Session-Less Sender ID size mismatch: "+QString::number(state.octomyProtocolSenderIDRawSize)+ " vs. "+QString::number(state.OCTOMY_SENDER_ID_SIZE);
			qWarning()<<es;
			emit commsError(es);
			return;
		}

		// Look up session for remote full ID
		QSharedPointer<CommsSession> session=lookUpSession(state.octomyProtocolSenderID);

		if(session.isNull() ) {
			// When session is missing and unless this is SYN, we drop the packet
			if( MULTIMAGIC_SYN != multimagic) {
				QString es="INFO: Dropping NON-SYN packet since we don't have session";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
		} else {
			const bool weWantToBeInitiator=session->handshakeState().isInitiator();

			// We both want to be the same role
			if(remoteWantsToBeInitiator == weWantToBeInitiator) {
				IDDuel duel(localID(), state.octomyProtocolSenderID);
				const bool weWin=duel.duel();
				// We are the winners, so we simply drop the packet and wait for the remote to get it (or not)
				if(weWin) {
					QString es="INFO: Dropping packet due to initiation conflict on the part of the remote";
					qWarning()<<es;
					emit commsError(es);
					return;
				}
				// We are the loosers so we simply adapt to the situation and continue as nothing happened.
				else {
					session->handshakeState().setInitiator(!weWantToBeInitiator);
					QString es="INFO: Dropping packet and ajusting due to initiation conflict on our part";
					qWarning()<<es;
					emit commsError(es);
					return;
				}
			}

			const auto step=multimagicToHandshakeStep(multimagic);
			const bool rxStepOK=(session->handshakeState().expectedStepRX() == step);
			if(rxStepOK) {
				bool rxOK=false;
				switch(multimagic) {
				case(MULTIMAGIC_SYN): {
					// We received SYN
					rxOK=recieveSyn(state);
					// TODO MOVE OK CHECK DOWN SINCE WE NEED OK TO UPDATE STEP
				}
				break;
				case(MULTIMAGIC_SYNACK): {
					// We received SYN-ACK
					rxOK=recieveSynAck(state, session);
				}
				break;
				case(MULTIMAGIC_ACK): {
					// We received SYN
					rxOK=recieveAck(state, session);
				}
				break;
				}
				if(rxOK) {
					session->handshakeState().handleRX(step);
					qDebug()<< "RX handshake state update was '" << rxOK << "' and state is now:" << session->handshakeState().toString();
				} else {
					qDebug()<< "RX handshake failed:" << session->handshakeState().toString();
				}

			} else {
				qDebug()<< "RX handshake expected step did not match incomming:" << session->handshakeState().toString();
			}

		}

	} else {
		qWarning()<<"ERROR: Could not receive encrypted body";
	}


}

/*
1. A sends SYN to B:
	Hi B. Here is my
	  + FULL-ID
	  + DESIRED SESSION-ID
	  + NONCE
	ENCODED WITH YOUR(B's) PUBKEY.
*/
bool CommsChannel::recieveSyn(PacketReadState &state)
{
	// REMOTE FULL-ID
	qDebug()<<"SYN RX FROM "<< state.octomyProtocolSenderID;

	// Create session for remote full ID
	QSharedPointer<CommsSession> session=createSession(state.octomyProtocolSenderID, false);
	if(nullptr==session) {
		QString es="ERROR: OctoMY Protocol Session could not be created for: " + state.octomyProtocolSenderID;
		qWarning()<<es;
		emit commsError(es);
		return false;
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
		return false;
	}
	qDebug()<< "SYN RX REMOTE DESIRED SESSION-ID WAS: "<< state.octomyProtocolDesiredRemoteSessionID;
	// REMOTE NONCE
	state.readEncRemoteNonce();
	if(state.octomyProtocolRemoteNonce == INVALID_NONCE) {
		QString es="ERROR: OctoMY Protocol remote nonce was invalid for SYN: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
		qWarning()<<es;
		emit commsError(es);
		return false;
	}
	qDebug()<< "SYN RX REMOTE NONCE WAS: "<< state.octomyProtocolRemoteNonce;

	// Record remote nonce
	session->setTheirLastNonce(state.octomyProtocolRemoteNonce);
	// Record remote desired session id
	session->setRemoteSessionID(state.octomyProtocolDesiredRemoteSessionID);
	return true;
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
bool CommsChannel::recieveSynAck(PacketReadState &state, QSharedPointer<CommsSession> session)
{
	// REMOTE FULL-ID
	qDebug()<<"SYN-ACK RX FROM "<< state.octomyProtocolSenderID;

	// REMOTE DESIRED SESSION-ID
	state.readEncDesiredRemoteSessionID();
	if(state.octomyProtocolDesiredRemoteSessionID < FIRST_STATE_ID ) {
		QString es="ERROR: OctoMY Protocol desired remote session ID not valid for SYN-ACK: "+QString::number(state.octomyProtocolDesiredRemoteSessionID);
		qWarning()<<es;
		emit commsError(es);
		return false;
	}
	qDebug()<<"SYN-ACK RX REMOTE DESIRED SESSION-ID: "<<state.octomyProtocolDesiredRemoteSessionID;

	// RETURN NONCE
	state.readEncReturnNonce();


	// Verify return nonce
	const SESSION_NONCE_TYPE expectedNonce=session->ourSynNonce();


	qDebug()<< "SYN-ACK RX RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce<<" vs. EXPECTED: "<<expectedNonce;

	if(state.octomyProtocolReturnNonce != expectedNonce) {
		QString es="ERROR: OctoMY Protocol return nonce " + QString::number(state.octomyProtocolReturnNonce)+" did not match expected " +QString::number(expectedNonce)+ " for SYN-ACK";
		qWarning()<<es;
		emit commsError(es);
		return false;
	}

	// REMOTE NONCE
	state.readEncRemoteNonce();
	qDebug()<< "SYN-ACK RX REMOTE NONCE WAS: "<< state.octomyProtocolRemoteNonce;

	// Verify remote nonce
	if(state.octomyProtocolRemoteNonce == INVALID_NONCE) {
		QString es="ERROR: OctoMY Protocol remote nonce " + QString::number(state.octomyProtocolRemoteNonce)+" was invalid";
		qWarning()<<es;
		emit commsError(es);
		return false;
	}

	// Record remote nonce
	session->setTheirLastNonce(state.octomyProtocolRemoteNonce);
	// Record remote desired session id
	session->setRemoteSessionID(state.octomyProtocolDesiredRemoteSessionID);
	return true;
}

/*
3. A answers ACK to B:
	Hi again B. HERE IS MY
	  + FULL-ID
	  + RETURN NONCE
	WELL RECEIVED.
*/
bool CommsChannel::recieveAck(PacketReadState &state, QSharedPointer<CommsSession> session)
{
	// REMOTE FULL-ID
	qDebug()<<"ACK RX FROM "<< state.octomyProtocolSenderID;

	// RETURN NONCE
	state.readEncReturnNonce();
	qDebug()<< "ACK RX RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce;

	// Verify return nonce
	const SESSION_NONCE_TYPE expectedNonce=session->ourAckNonce();
	if(state.octomyProtocolReturnNonce != expectedNonce) {
		QString es="ERROR: OctoMY Protocol return nonce " + QString::number(state.octomyProtocolReturnNonce)+" did not match expected " +QString::number(expectedNonce);
		qWarning()<<es;
		emit commsError(es);
		return false;
	}
	return true;
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
				qDebug()<<"GOT IDLE";
				// Idle packet does not contain any data, so we are done here :)
			}
			break;
			default:
			case(INVALID): {
				QString es=" ERROR: OctoMY message type invalid: "+QString::number((quint32)partMessageType,16);
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
						qWarning()<<"WARNING: SKIPPING "<<left<<" LEFTOVER BYTES AFTER COURIER WAS DONE";
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
	case(MULTIMAGIC_SYN):
	case(MULTIMAGIC_SYNACK):
	case(MULTIMAGIC_ACK): {
		recieveHandshake(state);
	}
	break;

	// No special purpose packet type was specified
	default: {
		recieveData(state);
	}
	break;
	}

}

void CommsChannel::doSendWithSession(PacketSendState &state)
{
	// Send data in stream
	if(nullptr==state.session) {
		qWarning()<<"ERROR: session was null in doSend";
		return;
	}
	const NetworkAddress na=state.session->address();
	if(!na.isValid(true,false)) {
		qWarning()<<"ERROR: invalid address: "	<<na;
		return;
	}
	const quint32 sz=state.datagram.size();
	if(sz<=0) {
		qWarning()<<"ERROR: datagram is <= 0 bytes ("<<state.datagram.size() <<")";
		return;
	}
	const qint64 written=mCarrier.writeData(state.datagram, na);
	//qDebug()<<"WROTE "<<written<<" bytes to "<<na.ip()<<":"<<na.port();
	if(written<0) {
		qDebug()<<"ERROR: in write for UDP SOCKET:"<<mCarrier.errorString()<< " for destination "<< na.toString()<<localID();
		return;
	} else if(written<sz) {
		qDebug()<<"ERROR: Only " << written << " of " <<sz<<" bytes of idle packet written to UDP SOCKET:"<<mCarrier.errorString()<< " for destination "<< na.toString()<<localID();
		return;
	}
	qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET";
	state.session->countSend(written);
}

////////////////////////////////////////////////////////////////////////////////


// Process existing sessions that are in handshake mode
// AND create new sessions for IDs that have not yet been enrolled
void CommsChannel::sendHandshake(const quint64 &now, const QString handShakeID)
{
	QSharedPointer<CommsSession> session=mSessions.getByFullID(handShakeID);
	if(nullptr==session) {
		qDebug()<<"CREATING NEW SESSION FOR ID "<< handShakeID<< " IN SEND HANDSHAKE";
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
		const HandshakeState &handshakeState=session->handshakeState();
		const HandshakeStep sendStep=handshakeState.expectedStepTX();
		const bool isInitiator=handshakeState.isInitiator();
		qDebug()<<"CURRENT HANDSHAKE STATE: "<<handshakeState.toString();
		switch(sendStep) {
		default:
		case(SYN): {
			//Verify that we are the initator in this session
			if(!isInitiator) {
				QString es="ERROR: OctoMY Protocol sending syn for session not initiated by us";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			sendSyn(state);
		}
		break;
		case(SYN_ACK): {
			//Verify that we are not initator in this session
			if(isInitiator) {
				QString es="ERROR: OctoMY Protocol sending syn-ack for session initiated by us";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			sendSynAck(state);
		}
		break;
		case(ACK): {
			//Verify that we are the initator in this session
			if(!isInitiator) {
				QString es="ERROR: OctoMY Protocol sending ack for session not initiated by us";
				qWarning()<<es;
				emit commsError(es);
				return;
			}
			sendAck(state);
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
	doSendWithSession(state);
	qDebug()<< "SYN TX handshake state is now :"<< state.session->handshakeState().toString();
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
	doSendWithSession(state);
	qDebug()<< "SYN-ACK TX handshake state is now :"<< state.session->handshakeState().toString();
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
	doSendWithSession(state);
	qDebug()<< "ACK TX handshake state is now :"<< state.session->handshakeState().toString();
}


////////////////////////////////////////////////////////////////////////////////



void CommsChannel::sendIdle(const quint64 &now, QSharedPointer<CommsSession> session)
{
	// NOTE: Session is needed because session holds the destination address
	if(nullptr==session) {
		qWarning()<<"ERROR: session was null";
		return;
	}
	PacketSendState state;
	state.writeMultimagic(MULTIMAGIC_IDLE);
	state.setSession(session);
	// NOTE: No need to get more fancy with the state, as the multimagic is the onlyrequired payload.
	doSendWithSession(state);
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
	const auto MINIMAL_PACKET_RATE=mCarrier.minimalPacketInterval();
	const auto MAXIMAL_PACKET_RATE=mCarrier.maximalPacketInterval();
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
					} else {
						if(session->handshakeState().isSending()) {
							mPendingHandshakes << id;
						}
					}
					// No previous session
				} else {


					/*

					If A sends first, A becomes initiator and B becomes adherent. All is dandy
					If B sends first, B becomes initiator and A becomes adherent. All is dandy
					If A & B send exqactly at the same time both A & B become initiator and chaos ensues

					This is resolved in the following manner:

					1. Detect if both A & B are initiator
					a. If we receive a packet indicating that the other party thinks they hold the same role as us (both initiator or both adherent)

					2. Look up ID-duel and the winner gets to keep current status while the looser must change.

					3. Drop the packet with a log entry and let the flow of handshake resume but now with both having correct role.


					Detect duplicate connection pairs in sessions, and remove the one that is inferior in ID-duel
					Time since first ever successfull connection
					Time since first ever connection attempt, successfull or not
					Time since last successfull connection
					Time since last unsuccessful connection attempt

					*/
					QSharedPointer<NodeAssociate> peer=mPeers.getParticipant(id);
					if(!peer.isNull()) {
						const quint64 timeSinceLastInitiatedHandshake=(now-peer->lastInitiatedHandshake());
						const quint64 timeSinceLastAdherentHandshake=(now-peer->lastAdherentHandshake());
						const quint64 timeSinceLastHandshake=qMin(timeSinceLastInitiatedHandshake, timeSinceLastAdherentHandshake);
						/*
						if(timeSinceLastHandshake> HANDSHAKE_GRACE_PERIOD){
							mPendingHandshakes << id;
						}


						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf

						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf
						sdløkfjsdkløfjsdkløfjsdkljf


						*/
					}
				}
			}
		}
	}
	// Ensure we get immediate sending opportunities for sessions that are in handshake
	if( (mPendingHandshakes.size()>0) || (mSessions.hasHandshakers()) ) {
		mMostUrgentSendingTime=qMin(mMostUrgentSendingTime, 0LL);
	}
	// Prepare for next sending opportunity
	const quint64 actualInterval = mCarrier.setDesiredOpportunityInterval(mMostUrgentSendingTime);
	return actualInterval;
}



//////////////////////////////////////////////////
// Send & receive slots



// NOTE: This is only for testing purposes! Real data should pass through the courier system
// and be dispatched by logic in sending timer
qint64 CommsChannel::sendRawData(QByteArray datagram, NetworkAddress address)
{
	return mCarrier.writeData(datagram, address);
}

NetworkAddress CommsChannel::localAddress()
{
	return mCarrier.address();
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
void CommsChannel::onCarrierReadyRead()
{
	qDebug()<<"----- READY READ for "<<localID();
	while (mCarrier.hasPendingData()) {
		qDebug()<<"      + UDP PACKET";
		QByteArray datagram;
		datagram.resize(mCarrier.pendingDataSize());
		QHostAddress host;
		quint16 port=0;
		qint64 ret=mCarrier.readData(datagram.data(), datagram.size(), &host, &port);
		if(ret>=0) {
			receivePacketRaw(datagram,host,port);
		} else {
			qWarning()<<"ERROR: error reading pending datagram";
		}
	}
}


void CommsChannel::onCarrierError(QString error)
{
	qDebug()<<"Carrier error in CC:"<<error;
	emit commsError(error);
}


void CommsChannel::onCarrierSendingOpportunity(const quint64 now)
{
	qDebug()<<"";
	qDebug()<<"--- SEND from "<< localID()<< " ---";
//	const quint64 now=QDateTime::currentMSecsSinceEpoch();
//	detectConnectionChanges(now);
//	mTXOpportunityRate.countPacket(0);
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
	qDebug()<<"SCHEDULED: "<<mSchedule.size();
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
	auto rate=mCarrier.minimalPacketInterval();
	auto lastActive= now-rate;
	auto idles=mSessions.getByIdleTime(lastActive);
	const int isz=idles.size();
	qDebug()<<"IDLES: "<< isz<<" (rate= "<<QString::number(rate)<<")";
	if(isz>0) {
		for(QSharedPointer<CommsSession> session:idles) {
			if(nullptr!=session) {
				sendIdle(now, session);
			} else {
				qWarning()<<"ERROR: session was null when sending idle packet";
			}
		}
	}
	rescheduleSending(now);
}

void CommsChannel::onCarrierConnectionStatusChanged(const bool connected)
{
	emit commsConnectionStatusChanged(connected);
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
