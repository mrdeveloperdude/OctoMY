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
 *
 */

class Client{
	public:
		ClientSignature signature;
		LogDestination *log;
		ReliabilitySystem reliabilitySystem;
		FlowControl flowControl;
		qint64 lastSendTime;
		qint64 lastReceiveTime;
		bool connected;
		bool lastConnected;
		float timeoutAccumulator;
		float deltaTime;
		float idleAccumulator;
		quint32 idlePacketsSent;


	public:

		//Client(QHostAddress host, quint16 port, LogDestination *log=0);
		Client(ClientSignature signature, LogDestination *log=0);
		void countSend(qint64 written);
		void receive();
		bool idle();
		void appendLog(QString);
		QString getSummary(QString sep="\n") const ;
		QString toString() const ;
		const QString getListText() const;
		//quint64 getHash() const;

		quint64 getShortHandID() const;



		//void onStatusMessage(SensorsMessage sm);
};



#endif // CLIENT_HPP

