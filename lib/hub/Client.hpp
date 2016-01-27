#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "comms/ReliabilitySystem.hpp"
#include "comms/FlowControl.hpp"
#include "comms/messages/StatusMessage.hpp"

#include <QHostAddress>


class LogDestination;

class Client{
	public:
		const quint64 fingerprint;
		const QHostAddress host;
		const quint16 port;
		//const quint64 hash;

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
		Client(quint64 fingerprint, QHostAddress host, quint16 port, LogDestination *log=0);
		void send(qint64 written);
		void receive();
		bool idle();
		void appendLog(QString);
		QString getSummary(QString sep="\n") const ;
		const QString getListText() const;
		quint64 getHash();


	signals:
		void onStatusMessage(StatusMessage sm);
};

#endif // CLIENT_HPP

