#ifndef COMMSSESSION_HPP
#define COMMSSESSION_HPP

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"
//#include "messages/SensorsMessage.hpp"
#include "comms/CommsSignature.hpp"


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
 * The Client class thus represents a communication partner that could, would,
 * will, does and did communicate with us through a CommsChannel.
 * \note not related to NodeAssociate, as this is tightly integrated with CommsChannel
 *
 */

class CommsSession
{
private:
	CommsSignature mSignature;
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




public:

	//Client(QHostAddress host, quint16 port, LogDestination *log=0);
	explicit CommsSession(CommsSignature signature, Key &key);
	virtual ~CommsSession();


	// Selectors
public:

	CommsSignature &signature();
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

public:

	void setExpired();
	void setEstablished();
	void countSend(qint64 written);
	void receive();
	bool idle();
	QString summary(QString sep="\n") const ;
	QString toString() const ;
	const QString listText() const;
	quint64 lastActiveTime() const;
	quint64 getShortHandID() const;

};



#endif // COMMSSESSION_HPP
