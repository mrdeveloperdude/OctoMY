#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"
//#include "messages/SensorsMessage.hpp"
#include "comms/ClientSignature.hpp"

#include <QHostAddress>
#include <QMap>


class LogDestination;

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
 * TODO: Rename this class to "PeerData" or similar?
 *
 */

class Client
{
private:
	ClientSignature mSignature;
	LogDestination *mLog;
	ReliabilitySystem mReliabilitySystem;
	FlowControl mRlowControl;
	qint64 mLastSendTime;
	qint64 mLastReceiveTime;
	bool mConnected;
	bool mLastConnected;
	float mTimeoutAccumulator;
	float mDeltaTime;
	float mIdleAccumulator;
	quint32 mIdlePacketsSent;


public:

	//Client(QHostAddress host, quint16 port, LogDestination *log=0);
	Client(ClientSignature signature, LogDestination *log=nullptr);


public:


	inline ClientSignature &signature()
	{
		return mSignature;
	}

	inline LogDestination *log()
	{
		return mLog;
	}
	inline ReliabilitySystem &reliabilitySystem()
	{
		return mReliabilitySystem;
	}

	inline FlowControl flowControl()
	{
		return mRlowControl;
	}
	inline qint64 lastSendTime()
	{
		return mLastSendTime;
	}

	inline qint64 lastRecieveTime()
	{
		return mLastReceiveTime;
	}

	inline bool connected()
	{
		return mConnected;
	}

	inline bool lastConnected()
	{
		return mLastConnected;
	}

	inline float timeoutAccumulator()
	{
		return mTimeoutAccumulator;
	}
	inline float deltaTime()
	{
		return mDeltaTime;
	}

	inline float idleAccumulator()
	{
		return mIdleAccumulator;
	}
	inline quint32 idlePacksSent()
	{
		return mIdlePacketsSent;
	}



public:


	void countSend(qint64 written);
	void receive();
	bool idle();
	void appendLog(QString);
	QString summary(QString sep="\n") const ;
	QString toString() const ;
	const QString listText() const;
	//quint64 getHash() const;

	quint64 lastActiveTime() const;

	quint64 getShortHandID() const;



	//void onStatusMessage(SensorsMessage sm);
};



#endif // CLIENT_HPP
