#ifndef COMMSSESSION_HPP
#define COMMSSESSION_HPP

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"
//#include "messages/SensorsMessage.hpp"
#include "NetworkAddress.hpp"

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

	quint64 mSessionID;
	QString mFullID;
	NetworkAddress mAddress;

	Key &mKey;
	ReliabilitySystem mReliabilitySystem;
	FlowControl mFlowControl;
	qint64 mLastSendTime;
	qint64 mLastReceiveTime;
	bool mConnected;
	bool mLastConnected;
	qreal mDisconnectTimeoutAccumulator;
	qreal mDisconnectTimeout;
	quint64 mExpireTimeoutAccumulator;
	quint64 mExpireTimeout;


	float mDeltaTime;
	float mIdleAccumulator;
	quint32 mIdlePacketsSent;
	bool mExpired;

	bool mEstablished;


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
	explicit CommsSession(QString fullID, Key &key);
	virtual ~CommsSession();


	// Selectors
public:

	//CommsSignature &signature();
	ReliabilitySystem &reliabilitySystem();
	FlowControl &flowControl();
	qint64 lastSendTime();
	qint64 lastRecieveTime();
	bool connected();
	bool lastConnected();
	float timeoutAccumulator();
	float deltaTime();
	float idleAccumulator();
	quint32 idlePacksSent();
	Key &key();
	bool expired();
	bool established();

	// CommsSignature replacements:
public:

	quint64 sessionID() const;
	QString fullID() const;
	NetworkAddress address() const;

public:

	void setExpired();
	void setEstablished();
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
