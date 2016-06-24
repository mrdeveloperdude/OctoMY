#ifndef COMMSCHANNEL_HPP
#define COMMSCHANNEL_HPP

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QList>
#include <QTimer>
#include <QDateTime>

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"

#include "basic/LogDestination.hpp"
#include "couriers/Courier.hpp"

#define TIMEOUT_TRESHOLD 1

#define OCTOMY_PROTOCOL_MAGIC (0x0C701111)
#define OCTOMY_PROTOCOL_VERSION_CURRENT (1)
#define OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT (QDataStream::Qt_5_5)



class HubWindow;
class Client;
class Courier;
class ClientDirectory;
class ClientSignature;

/*
Architecture:

TCP tries to pretend on behalf of the user that network traffic is a dependable
linear unbroken stream of bytes. From the user's perspective this has the
benefit of being easy to understand and use. The two major downsides however
are that:

1. This far from how a network actually works and so it is hard to make TCP work
   it has taken decades of evolution for TCP to become as good as it is today,
   but it is still limited by this fallacy

2. Some needs may actually be better met by not thinking about the network as a
   linear stream of bytes, so going the length to pretend that it is actually
   just gets in the way.

CommChannel is an API wrapping UDP in Qt in a way that allows the user to
exploit the benefits of communications over UDP by modelling closesly the
benefits in a way that hides their inherent complexities.

The CommChannel API works like this:

1. Users of the API register "couriers" that each is responsible for keeping the
   latest data fresh and ready for sending should an opportunity present itself.
   Couriers each tend to a certain type of packet with a certain priority and
   desired sending frequency. It is up to each courier to maintain it's own
   state.

2. CommChannel is in charge and decides the speed at which packets are sent and
   which couriers get their packets sent on each opportunity.

3. CommChannel may at any time send non-payload data in each packet or even
   special purpose network-only packets to sustain its operation. If there is no
   data to be sent by couriers, CommChannel may send no-op packets that
   facilitates the calculation of the network characteristics such as
   round trip time. This is done transparentyl to the couriers.


Notes:

1. CommChannel binds to a local address and port, but does not really
   discriminate from where inbound traffic arrives. All packets are treated
   equal as all packes contain identification of the source.

*/

class CommsChannel : public QObject{
		Q_OBJECT
	private:
		static const quint64 CONNECTION_TIMEOUT=1000*5;//5 sec connection timeout
		QUdpSocket udpSocket;
		QTimer sendingTimer;
		//Clients as identifued by their fingerprints
		ClientDirectory *clients;
		LogDestination *mw;
		//All registered couriers
		QList<Courier *> couriers;
		//const quint32 localClientPlatform;		const quint32 localClientExecutable;
		ClientSignature localSignature;
		quint64 lastRX;
		quint64 lastTX;
		quint64 lastRXST;
		quint64 lastTXST;
		quint64 sendCount;
		quint64 recCount;
		bool connected;
	public:

		explicit CommsChannel(QObject *parent=nullptr, LogDestination *mw=nullptr);

	public:

		ClientDirectory *getClients();

		void start(QHostAddress bindAddress, quint16 bindPort);
		void stop();

		void setLogOutput(LogDestination *mw);

		QString getSummary();



		void hookSignals(QObject &ob);
		void unHookSignals(QObject &ob);

		void registerCourier(Courier &);
		void unregisterCourier(Courier &);

		bool isConnected(){return connected;}

		qint64 sendRawData(QByteArray datagram,ClientSignature sig);


	private:

		void appendLog(QString);

		// CommsChannel signals
	signals:
//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
		void error(QString message);
		void clientAdded(Client *c);
		void connectionStatusChanged(bool c);

	private slots:

		void receivePacketRaw(QByteArray ba,QHostAddress host, quint16 port);
		void onSendingTimer();

	public slots:

		void onReadyRead();
		void onUdpError(QAbstractSocket::SocketError);
};

#endif // COMMSCHANNEL_HPP