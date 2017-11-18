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
#include "basic/AddressList.hpp"
#include "comms/CommsSession.hpp"
#include "messages/MessageType.hpp"
#include "security/KeyStore.hpp"
#include "pose/Pose.hpp"
#include "discovery/AddressBook.hpp"
#include "basic/AddressEntry.hpp"

#include "PacketSendState.hpp"
#include "PacketReadState.hpp"
#include "IDDuel.hpp"

#include <QDataStream>
#include <QDateTime>
#include <QSharedPointer>

#define FIRST_STATE_ID ((SESSION_ID_TYPE)MULTIMAGIC_LAST)
#define MAX_CONCURRENT_COURIERS (100)

//#define DO_CC_ENC

CommsChannel::CommsChannel(CommsCarrier &carrier, KeyStore &keystore, AddressBook &peers, QObject *parent)
	: QObject(parent)
	, mCarrier(carrier)
	, mKeystore(keystore)
	, mAssociates(peers)
	, mSessions(mKeystore)
	, mLocalSessionID(0)
	, mTXScheduleRate("CC TX SCHED")
	, mHoneyMoonEnd(0)
//	, mConnected(false)
{
	OC_METHODGATE();
	setObjectName("CommsChannel");
	mCarrier.setHookCarrierSignals(*this, true);
}

/*
//TODO: Remove once nobody refers to it any more
CommsChannel::CommsChannel(CommsCarrier &carrier, KeyStore &keystore, QObject *parent)
	: QObject(parent)
	, mCarrier(carrier)
	, mKeystore(keystore)
	, mAssociates(*((NodeAssociateStore *) nullptr ))//HACK
	, mSessions(mKeystore)
{
	qWarning()<<"ERROR: PLEASE USE THE OTHER CONSTRUCTOR AS THIS IS UNSUPPORTEDF!";
}
*/

CommsChannel::~CommsChannel()
{
	OC_METHODGATE();
	//mCarrier.setHookCarrierSignals(*this, false);
}





////////////////////////////////////////////////////////////////////////////////



CommsCarrier &CommsChannel::carrier()
{
	OC_METHODGATE();
	return mCarrier;
}

CommsSessionDirectory &CommsChannel::sessions()
{
	OC_METHODGATE();
	return mSessions;
}



bool CommsChannel::recieveEncryptedBody(PacketReadState &state)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	QSharedPointer<CommsSession> session(nullptr);
	if(mKeystore.hasPubKeyForID(id)) {
		auto key=mKeystore.pubKeyForID(id);
		if(nullptr!=key) {
			if(key->isValid(true)) {
				SESSION_ID_TYPE localSessionID=mSessions.generateUnusedSessionID();
				if(localSessionID >= FIRST_STATE_ID ) {


					QSharedPointer<Associate> associate=mAssociates.associateByID(id);
					if(nullptr!=associate) {
						QSharedPointer<AddressEntry>  ae=associate->addressList().highestScore();
						if(!ae.isNull()) {
							NetworkAddress addr=ae->address;
							if(addr.isValid()) {
								session=QSharedPointer<CommsSession>(OC_NEW CommsSession(key));
								OC_ASSERT(nullptr!=session);
								if(nullptr!=session) {
									//session->setRemoteSessionID(desiredRemoteSessionID);
									session->handshakeState().setInitiator(initiator);
									session->setLocalSessionID(localSessionID);


									//associate->localAddress()

									session->setAddress(addr);
									// Generate our syn nonce
									const SESSION_NONCE_TYPE synNonce=session->createOurSynNonce();
									qDebug()<<"OUR SYN TX NONCE CREATED: "<<synNonce;

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
							} else {
								qWarning()<<"ERROR: address was invalid when creating comms session for ID "+id;
							}
						} else {
							qWarning()<<"ERROR: address was null when creating comms session for ID "+id;
						}
					} else {
						QString es="ERROR: no associate found for ID "+id;
						qWarning()<<es;
						emit commsError(es);
					}
				} else {
					QString es="ERROR: OctoMY Protocol local session ID not valid: "+QString::number(localSessionID);
					qWarning()<<es;
					emit commsError(es);
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
	OC_METHODGATE();
	// Look for existing sessions tied to this ID
	QSharedPointer<CommsSession> session=mSessions.byFullID(id);
	return session;
}



void CommsChannel::recieveIdle(PacketReadState &state)
{
	OC_METHODGATE();
	qDebug()<<"RECEIVED IDLE PACKET";
}



void CommsChannel::recieveHandshake(PacketReadState &state)
{
	OC_METHODGATE();
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
			} else {
				// Create session for remote full ID
				session=createSession(state.octomyProtocolSenderID, false);
				if(nullptr==session) {
					QString es="ERROR: OctoMY Protocol Session could not be created for: " + state.octomyProtocolSenderID+ " in receive handshake";
					qWarning()<<es;
					emit commsError(es);
					return;
				}
			}
		}
		if(!session.isNull() ) {
			const bool weWantToBeInitiator=session->handshakeState().isInitiator();

			// We both want to be the same role
			if(remoteWantsToBeInitiator == weWantToBeInitiator) {
				qDebug()<<"DUEL: "<< remoteWantsToBeInitiator << " vs. "<<  weWantToBeInitiator;
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
					rxOK=recieveSyn(state, session);
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
				qDebug()<< "RX handshake expected step '"<< handshakeSendStepToString(step) << "' did not match incomming:" << session->handshakeState().toString();
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
bool CommsChannel::recieveSyn(PacketReadState &state, QSharedPointer<CommsSession> session)
{
	OC_METHODGATE();
	// REMOTE FULL-ID
	qDebug()<<"SYN RX FROM "<< state.octomyProtocolSenderID;

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

	// (Re)generate our syn-ack nonce
	const SESSION_NONCE_TYPE synAckNonce=session->createOurSynAckNonce();
	qDebug()<<"OUR SYN-ACK TX NONCE CREATED: "<<synAckNonce;

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
	OC_METHODGATE();
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

	// (Re)generate our ack nonce
	const SESSION_NONCE_TYPE ackNonce=session->createOurAckNonce();
	qDebug()<<"OUR ACK TX NONCE CREATED: "<<ackNonce;

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
	OC_METHODGATE();
	// REMOTE FULL-ID
	qDebug()<<"ACK RX FROM "<< state.octomyProtocolSenderID;

	// RETURN NONCE
	state.readEncReturnNonce();
	qDebug()<< "ACK RX RETURN NONCE WAS: "<< state.octomyProtocolReturnNonce;

	// Verify return nonce
	const SESSION_NONCE_TYPE expectedNonce=session->ourAckNonce();
	if(state.octomyProtocolReturnNonce != expectedNonce) {
		QString es="ERROR: OctoMY Protocol return nonce " + QString::number(state.octomyProtocolReturnNonce)+" did not match expected " +QString::number(expectedNonce)+ " for ACK";
		qWarning()<<es;
		emit commsError(es);
		return false;
	}

	return true;
}



void CommsChannel::recieveData(PacketReadState &state)
{
	OC_METHODGATE();
	qDebug()<<"RX DATA";
	SESSION_ID_TYPE sessionID = (SESSION_ID_TYPE)state.multimagic;
	if(sessionID < FIRST_STATE_ID) {
		QString es="ERROR: invalid multimagic specified";
		qWarning()<<es;
		emit commsError(es);
		return;
	}
	auto session=mSessions.bySessionID(sessionID);
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
	// TODO: Verify if this is the right place to set "last seen"
	const QString id = session->fullID();
	QSharedPointer<Associate> peer=mAssociates.associateByID(id);
	if(!peer.isNull()) {
		qDebug()<< "SET LAST SEEN FOR " << id;
		peer->setLastSeen();
	}
	quint16 partsCount = 0;
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
			QSharedPointer<Courier> courier=mCouriersByID.value(state.partMessageTypeID, nullptr);
			if(!courier.isNull()) {
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
	OC_METHODGATE();
	qDebug()<<"";
	qDebug()<<"--- RECEIVE to "<< localID()<< " ---";
	PacketReadState state(datagramS, remoteHostS, remotePortS);

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
	OC_METHODGATE();
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
		qDebug()<<"ERROR: while writing "<<sz<<" bytes for UDP SOCKET:"<<mCarrier.errorString()<< " for destination "<< na.toString()<<localID();
		return;
	} else if(written<sz) {
		qDebug()<<"ERROR: Only " << written << " of " <<sz<<" bytes of idle packet written to UDP SOCKET:"<<mCarrier.errorString()<< " for destination "<< na.toString()<<localID();
		return;
	}
	qDebug()<<"WROTE "<<written<<" BYTES TO UDP SOCKET FOR "<<na.toString();
	state.session->countSend(written);
}

////////////////////////////////////////////////////////////////////////////////


// Process existing sessions that are in handshake mode
// AND create new sessions for IDs that have not yet been enrolled
void CommsChannel::sendHandshake(const quint64 &now, const QString handShakeID)
{
	OC_METHODGATE();
	QSharedPointer<CommsSession> session=mSessions.byFullID(handShakeID);
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
		session->handshakeState().handleTX(sendStep);
		qDebug()<< "TX handshake was ok and state is now:" << session->handshakeState().toString();

	} else {
		QString es="ERROR: OctoMY Protocol session could not be created for: " + handShakeID+" in send handshake";
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
	OC_METHODGATE();
	// Send
	state.writeMultimagic(MULTIMAGIC_SYN);
	// FULL-ID
	state.writeEncSenderID(localID());
	// DESIRED SESSION-ID
	const SESSION_ID_TYPE localSessionID=state.session->localSessionID();
	qDebug()<<"SYN TX LOCAL SESSION ID="<< localSessionID;
	state.writeEncSessionID(localSessionID);
	// NONCE
	const SESSION_NONCE_TYPE synNonce=state.session->ourSynNonce();
	qDebug()<<"SYN TX OUR NONCE="<<synNonce;
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
	OC_METHODGATE();
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
	const SESSION_NONCE_TYPE synAckNonce=state.session->ourSynAckNonce();
	qDebug()<<"SYN-ACK TX OUR NONCE : "<<synAckNonce;
	state.writeEncNonce(synAckNonce);
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
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	mTXScheduleRate.countPacket(0);
	const auto MINIMAL_PACKET_RATE=mCarrier.minimalPacketInterval();
	const auto MAXIMAL_PACKET_RATE=mCarrier.maximalPacketInterval();
	qDebug()<<" ... R E S C H E D U L E ... "<<now;
	mMostUrgentSendingTime=MINIMAL_PACKET_RATE;
	mSchedule.clear();
	// Update first
	for(QSharedPointer<Courier> courier:mCouriers) {
		if(!courier.isNull()) {
			courier->update(now);
		}
	}
	mPendingHandshakes.clear();
	// Re-schedule couriers (strive to adhere to the mandates set fourth by each courier)
	for(QSharedPointer<Courier> courier:mCouriers) {
		if(!courier.isNull()) {
			CourierMandate cm=courier->mandate();
			//qDebug()<<"MANDATE FOR "<<courier->name()<<" IS "<<cm.toString();
			if(cm.sendActive) {
				// See if the node for this courier has been enrolled yet
				const QString id=courier->destination();
				QSharedPointer<CommsSession> session=mSessions.byFullID(id);
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

					QSharedPointer<Associate> peer=mAssociates.associateByID(id);
					if(!peer.isNull()) {
						const quint64 timeSinceLastInitiatedHandshake=(now - peer->lastInitiatedHandshake());
						const quint64 timeSinceLastAdherentHandshake=(now - peer->lastAdherentHandshake());
						const quint64 timeSinceLastHandshake=qMin(timeSinceLastInitiatedHandshake, timeSinceLastAdherentHandshake);

						quint64 gracePeriod = 0;
						if(honeymoon(now)) {
							gracePeriod = 0;
						}

						if(timeSinceLastHandshake > gracePeriod) {
							qDebug()<<"#### Initiating handshake for "<<id;
							mPendingHandshakes << id;
						}

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
	OC_METHODGATE();
	return mCarrier.writeData(datagram, address);
}

void CommsChannel::setHoneymoonEnd(quint64 hEndMS)
{
	OC_METHODGATE();
	const quint64 iv=QDateTime::currentMSecsSinceEpoch() - hEndMS;
	if(iv>0) {
		qDebug()<<"HONEYMOON ENABLED FOR "<<utility::humanReadableElapsedMS(iv);
	} else {
		qDebug()<<"HONEYMOON DISABLED";
	}
	mHoneyMoonEnd = hEndMS;
}

bool CommsChannel::honeymoon(quint64 now)
{
	OC_METHODGATE();
	if(0==now) {
		now= QDateTime::currentMSecsSinceEpoch();
	}
	return mHoneyMoonEnd > now;
}



// Report if this commschannel would rather be connected or not (registered couriers > 0)
bool CommsChannel::needConnection()
{
	OC_METHODGATE();
	return mCouriers.count()>0;
}

// [Dis]connect based on our needConnection()
void CommsChannel::updateConnect()
{
	OC_METHODGATE();
	const bool is=mCarrier.isConnected();
	const bool needs = needConnection();
	if(needs != is) {
		qDebug()<<"Comms channel update decided to start carrier";
		mCarrier.setStarted(needs);
	}
}


NetworkAddress CommsChannel::localAddress()
{
	return mCarrier.address();
}

QString CommsChannel::localID()
{
	OC_METHODGATE();
	auto key=mKeystore.localKey();
	if(nullptr==key) {
		return "";
	}
	return key->id();
}

QString CommsChannel::getSummary()
{
	OC_METHODGATE();
	QString out;
	for(QSharedPointer<Courier> courier:mCouriers) {
		if(courier.isNull()) {
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	qDebug()<<"Carrier error in CC:"<<error;
	emit commsError(error);
}


void CommsChannel::onCarrierSendingOpportunity(const quint64 now)
{
	OC_METHODGATE();
	const auto ctHandshake =mPendingHandshakes.size();
	const auto ctScheduled=mSchedule.size();
	const auto rate=mCarrier.minimalPacketInterval();
	const auto lastActive= now-rate;
	const auto idles=mSessions.byIdleTime(lastActive);
	const int isz=idles.size();
	qDebug()<<"";
	qDebug()<<" ### SENDING OPPERTUNITY from "<< localID()<< " HANDSHAKE: "<<ctHandshake<<", CARRIER: "<<ctScheduled<<", IDLE: "<< isz<<" (rate= "<<QString::number(rate)<<")";
//	const quint64 now=QDateTime::currentMSecsSinceEpoch();
//	detectConnectionChanges(now);
//	mTXOpportunityRate.countPacket(0);
	// First look at handshakes
	for(QSet<QString> ::const_iterator i=mPendingHandshakes.begin(); i!= mPendingHandshakes.end(); ) {
		const QString handShakeID=(*i);
		//qDebug() << " + " << handShakeID;
		sendHandshake(now, handShakeID);
		// Increase counter here to ensure that any removed items don't crash our loop
		++i;
	}
	// Next look at courier schedule
	for (QMap<quint64, QSharedPointer<Courier> >::iterator i = mSchedule.begin(); i != mSchedule.end(); ) {
		QSharedPointer<Courier> courier=i.value();
		//qDebug() << " + " << i.key() << " = " << ((nullptr==courier)?"NULL":courier->toString());
		// Increase counter here to ensure that any removed items don't crash our loop
		++i;
	}
	// ... and send data according to schedule
	for (QMap<quint64, QSharedPointer<Courier> >::iterator i = mSchedule.begin(); i != mSchedule.end(); ) {
		QSharedPointer<Courier> courier=i.value();
		if(!courier.isNull()) {
			sendCourierData(now, *courier);
		} else {
			qWarning()<<"WARNING: scheduled courier was null";
		}
		// Increase counter here to ensure that any removed items don't crash our loop
		++i;
	}
	// Finally send idle packets to sessions that need it
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
	OC_METHODGATE();
	emit commsConnectionStatusChanged(connected, mCouriers.commsEnabled(false));
}


void CommsChannel::appendLog(QString msg)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	qRegisterMetaType<CommsSession *>("Client *");
	if(hook) {
		if(!connect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(commsClientAdded(CommsSession *)),&ob,SLOT(onCommsClientAdded(CommsSession *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(commsConnectionStatusChanged(bool, bool)),&ob,SLOT(onCommsConnectionStatusChanged(bool, bool)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(this,SIGNAL(commsError(QString)),&ob,SLOT(onCommsError(QString)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(commsClientAdded(CommsSession *)),&ob,SLOT(onCommsClientAdded(CommsSession *)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(commsConnectionStatusChanged(bool, bool)),&ob,SLOT(onCommsConnectionStatusChanged(bool, bool)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}




void CommsChannel::setHookCourierSignals(QSharedPointer<Courier> courier, bool hook)
{
	OC_METHODGATE();
	if(!courier.isNull()) {
		if(hook) {
			if(!connect(courier.data(), SIGNAL(reschedule(quint64)),this,SLOT(rescheduleSending(quint64)),OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect Courier::reschedule for "<<courier->name();
			}

		} else {
			if(!disconnect(courier.data(), SIGNAL(reschedule(quint64)),this,SLOT(rescheduleSending(quint64)))) {
				qWarning()<<"ERROR: Could not disconnect Courier::reschedule for "<<courier->name();
			}
		}
	}
}

/*
bool CommsChannel::courierRegistration()
{
	OC_METHODGATE();
	return mCouriers.commsEnabled();
}
*/

// [Un]register courier to NODE
void CommsChannel::setCourierRegistered(QSharedPointer<Courier> courier, bool reg)
{


	/*
		//qDebug()<<"COMMS LEFT WITH "<<ct<<" COURIERS";
		//qDebug()<< cc->getSummary();
		// Adaptively start commschannel when there are couriers registered
		const int ct=courierCount();

		if(ct>0 && mWantToConnect) {
			startComms();
		} else {
			if( isCommsStarted() )  {
				//qDebug()<<"STOPPING COMMS ";
				stopComms();
			} else {
				//qDebug()<<"COMMS ALREADY STOPPED";
			}
		}
		*/



	OC_METHODGATE();
	if(!courier.isNull()) {
		const bool ok=mCouriers.setRegistered(courier, reg);
		if(ok) {
			const quint32 id=courier->id();
			if(reg) {
				mCouriersByID[id]=courier;
			} else {
				mCouriersByID.remove(id);
			}
		}
		setHookCourierSignals(courier, (ok || !reg));
	}
}

CourierSet CommsChannel::couriers()
{
	OC_METHODGATE();
	return mCouriers;
}

/* TODO: repalce these by calls to couriers()
int CommsChannel::courierCount()
{
	OC_METHODGATE();
	return mCouriers.count();
}



bool CommsChannel::hasCourier(Courier &c) const
{
	OC_METHODGATE();
	QSharedPointer<Courier> p(&c);
	return mCouriers.contains(p);
}

Courier *CommsChannel::getCourierByID(quint32 id) const
{
	OC_METHODGATE();
	if(mCouriersByID.contains(id)) {
		return mCouriersByID[id];
	}
	return nullptr;
}
*/



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
