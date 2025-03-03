#ifndef COMMSSESSION_HPP
#define COMMSSESSION_HPP

#include "CommsSessionIDType.hpp" // For SESSION_ID_TYPE
#include "FlowControl.hpp"
#include "HandshakeState.hpp"
#include "ReliabilitySystem.hpp"
#include "address/CarrierAddress.hpp"
//#include "messages/SensorsMessage.hpp"

#include <QHostAddress>
#include <QMap>



class Key;

/**
 * \brief Since communication with CommsChannel implies a level of uncertainty
 * of when we are "connected" we simply keep a list of recent communication
 * partners with state such as:
 *
 *  * last seen timestamp
 *  * address
 *  * port
 *  * unique fingerprint
 *  * reliability state
 *  * flow control state
 *
 * The CommsSession class thus represents a communication partner that could, would,
 * will, does or did communicate with us through a CommsChannel.
 * \note compared to NodeAssociate, a session is more tightly integrated with CommsChannel
 * and less persistent.
 *
 */


class CommsSession
{
private:
	//CommsSignature mSignature;

	QSharedPointer<Key> mKey;
	SESSION_ID_TYPE mLocalSessionID; // What the local side of this session has decided is the ID for itself
	SESSION_ID_TYPE mRemoteSessionID; // What the remote side of this session has decided is the ID for itself
	QSharedPointer<CarrierAddress> mAddress;


	ReliabilitySystem mReliabilitySystem;
	FlowControl mFlowControl;
	qint64 mLastSendTime;
	qint64 mLastReceiveTime;
	qreal mDisconnectTimeoutAccumulator;
	qreal mDisconnectTimeout;
	quint64 mExpireTimeoutAccumulator;
	quint64 mExpireTimeout;


	float mDeltaTime;
	float mIdleAccumulator;
	quint32 mIdlePacketsSent;

	bool mConnected; // Is there a "connection" now?
	bool mLastConnected; // Internal helper
	bool mExpired; // Has this session been terminated?


	HandshakeState mHandshakeState;


	bool mInitialized;


#ifdef USE_RELIBABILITY_SYSTEM
	quint32 packet_sequence=0;
	quint32 packet_ack=0;
	quint32 packet_ack_bits=0;
#endif


#ifdef USE_RELIBABILITY_SYSTEM
	*ds >> packet_sequence;
	totalAvailable-=sizeof(packet_sequence);
	*ds >> packet_ack;
	totalAvailable-=sizeof(packet_ack);
	*ds >> packet_ack_bits;
	totalAvailable-=sizeof(packet_ack_bits);
//qDebug()<<totalRecCount<<"Data received from client '"<<remoteSignature.toString()<<"' with seq="<<packet_sequence<<" ack="<<packet_ack<<" bits="<<packet_ack_bits<<" and bodysize="<<totalAvailable;
	ReliabilitySystem &rs=remoteClient->reliabilitySystem();
	rs.packetReceived( packet_sequence, size-expectedHeaderSize  );
	rs.processAck( packet_ack, packet_ack_bits );
#endif


	//NONCES

	SESSION_NONCE_TYPE mOurSynNonce;
	SESSION_NONCE_TYPE mOurSynAckNonce;
	SESSION_NONCE_TYPE mOurAckNonce;
	SESSION_NONCE_TYPE mTheirLastNonce;


public:

	//Client(QHostAddress host, quint16 port, LogDestination *log=0);
	explicit CommsSession(QSharedPointer<Key> key);
	virtual ~CommsSession();

	// Selectors
public:


	ReliabilitySystem &reliabilitySystem();
	FlowControl &flowControl();
	qint64 lastSendTime() const;
	qint64 lastRecieveTime() const;
	float timeoutAccumulator() const;
	float deltaTime() const;
	float idleAccumulator() const;
	quint32 idlePacksSent() const;
	QSharedPointer<Key> key() const;

	bool connected() const;
	bool lastConnected() const;
	HandshakeState &handshakeState();
	bool established() const;
	bool expired() const;

	// CommsSignature replacements:
public:

	SESSION_ID_TYPE localSessionID() const;
	SESSION_ID_TYPE remoteSessionID() const;
	QString fullID() const;
	QSharedPointer<CarrierAddress> address() const;

	void setLocalSessionID(SESSION_ID_TYPE );
	void setRemoteSessionID(SESSION_ID_TYPE );


	// nonce management
public:
	SESSION_NONCE_TYPE createOurSynNonce();
	SESSION_NONCE_TYPE createOurSynAckNonce();
	SESSION_NONCE_TYPE createOurAckNonce();
	SESSION_NONCE_TYPE ourSynNonce() const;
	SESSION_NONCE_TYPE ourSynAckNonce() const;
	SESSION_NONCE_TYPE ourAckNonce() const;
	void setTheirLastNonce(SESSION_NONCE_TYPE);
	SESSION_NONCE_TYPE theirLastNonce() const;

public:

	void setAddress(QSharedPointer<CarrierAddress> address);

	void setExpired();

	void countSend(qint64 written);
	void receive();
	bool idle();

	const QString signatureToString() const;

	QString summary(QString sep="\n") const ;
	QString toString() const ;
	const QString listText() const;
	quint64 lastActiveTime() const;


	QVariantMap toVariantMap();
	void fromVariantMap(const QVariantMap map);


};



#endif // COMMSSESSION_HPP
