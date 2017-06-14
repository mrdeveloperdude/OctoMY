#ifndef COMMSSESSION_HPP
#define COMMSSESSION_HPP

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"
//#include "messages/SensorsMessage.hpp"
#include "NetworkAddress.hpp"
#include "CommsSessionIDType.hpp" // For SESSION_ID_TYPE
#include "HandshakeState.hpp"

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

	Key &mKey;
	SESSION_ID_TYPE mLocalSessionID; // What the local side of this session has decided is the ID for itself
	SESSION_ID_TYPE mRemoteSessionID; // What the remote side of this session has decided is the ID for itself
	NetworkAddress mAddress;


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




public:

	//Client(QHostAddress host, quint16 port, LogDestination *log=0);
	explicit CommsSession(Key &key);
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
	Key &key() const;

	bool connected() const;
	bool lastConnected() const;
	bool handshakeStarted() const;
	bool established() const;
	HandshakeStep nextStep() const;
	bool expired() const;

	// CommsSignature replacements:
public:

	SESSION_ID_TYPE localSessionID() const;
	SESSION_ID_TYPE remoteSessionID() const;
	QString fullID() const;
	NetworkAddress address() const;

	void setLocalSessionID(SESSION_ID_TYPE );
	void setRemoteSessionID(SESSION_ID_TYPE );

public:

	void setAddress(const NetworkAddress &address);

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
