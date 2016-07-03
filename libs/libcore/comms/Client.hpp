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
		const QString getListText() const;
		quint64 getHash() const;



		//void onStatusMessage(SensorsMessage sm);
};



/**
  * \brief ClientDirectory holds all clients beknownst to us.
  *
  */

class ClientDirectory: public QObject{
		Q_OBJECT
	private:
		QMap<quint64, Client *> byHash;
		QMap<quint64, Client *> byHost;
		QSet<Client *> all;
	public:

		explicit ClientDirectory():QObject(0){}
	public:

		void insert(Client *c);
		Client *getByHash(const quint32 fingerprint, const quint32 nonce, const bool addIfMissing=false);
		Client *getByHost(const QHostAddress adr,const  quint16 port, const bool addIfMissing=false);
		Client *getBest(const ClientSignature signarture, const bool addIfMissing=false);
		Client *getBest(const quint32 platform, const quint32 executable, const QHostAddress host,const  quint16 port, const bool addIfMissing=false);

		int count();

	signals:
		void clientAdded(Client *);


};


#endif // CLIENT_HPP

