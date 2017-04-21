/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef COMMSCHANNEL_HPP
#define COMMSCHANNEL_HPP

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QDateTime>

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"

#include "basic/LogDestination.hpp"
#include "couriers/Courier.hpp"
#include "CommsSessionDirectory.hpp"

// It seems that 30 seconds would be a "safe" minimal UDP rate to avoid routers closing our "connection"
#define MINIMAL_PACKET_RATE 5000

#define OCTOMY_PROTOCOL_MAGIC (0x0C701111)
#define OCTOMY_PROTOCOL_VERSION_CURRENT (1)
#define OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT (QDataStream::Qt_5_7)



class HubWindow;
class CommsSession;
class CommsSessionDirectory;
class KeyStore;

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
   equal as all packes inherently contain an identification of the source.

2. Communication between remote and agent is initiated when user presses the "connect"
   buttons in the respective user interfaces.

3.  At that point agent will attemt to contact all trusted remotes at their last
   known address untill one or more answer are received. From then on, all
   connections that did not result in valid responses will be closed, and only
   retried periodically.

4. All Agent initiated communication will be broadcast to all active remotes in
   paralell. Remote initiated transfers will remain private.


Session management strategy:

 + Uppon the start of application, all entries in NodeAssociateStore are added to CommsSessionDirectory with their stored state. This ensures that session will persist.
 + Comms channel may be in "normal" or "honeymoon" mode where honeymoon mode is typically enabled in a period just after communications are enabled, or at users disgresion.
 + Comms channel in "honeymoon" mode will ping all nodes that are not active continuously at a short (seconds) interval until the honeymoon wears off.
 + Comms channel in "normal" mode will ping all nodes that are not active at an exponentially decaying frequency as a function of how long since their last recorded ping response.
 + Any node that replies to a ping are immideately upgraded to "active"
 + Any active node has a handshake started, and communication is maintained with them until completion or until the connection times out
 + If communication with an active node is explicitly completed, it will not receive pings, even during honeymoon, for a specified grace period
 + If communication with an active node simply times out it will instead continue to parttake in the pinging as normal.
 + The node that was last active is always treated as if it is no older than 1 hour, and will in inactive periods thus be pinged as often.

*/

class CommsChannel : public QObject
{
	Q_OBJECT
private:
	static const quint64 CONNECTION_TIMEOUT=MINIMAL_PACKET_RATE +1000;//1 sec  more than our UDP timeout
	// NOTE: We use 512 as the maximum practical UDP size for ipv4 over the internet
	//       See this for discussion: http://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
	static const qint32 MAX_UDP_PAYLOAD_SIZE=512;

	QUdpSocket mUDPSocket;
	QTimer mSendingTimer;
	KeyStore &mKeystore;
	//Clients as identifued by their fingerprints
	CommsSessionDirectory mSessions;
	//All registered couriers
	QList<Courier *> mCouriers;
	QMap<quint32, Courier *> mCouriersByID;
	QString mLocalID;
	quint64 mLocalSessionID;
	NetworkAddress mLocalAddress;
	//CommsSignature mLocalSignature;
	quint64 mLastRX;
	quint64 mLastTX;
	quint64 mLastRXST;
	quint64 mLastTXST;
	quint64 mTxCount;
	quint64 mRxCount;
	bool mConnected;
	// When honeymoon mode is enabled, all inactive associates are pinged continuously in an effort to start new connections
	bool mHoneyMoon;

	QMap<quint64, Courier *> mPri;
	QList <const QString *> mIdle;
	qint64 mMostUrgentCourier;


	//Receive counter used in debug messages to make sense of the order of things
	static quint32 sTotalRecCount;

public:

	explicit CommsChannel(const QString &id, KeyStore &keystore, QObject *parent=nullptr);

	private:

	void countReceived();

public:

	CommsSessionDirectory &sessions();
	void start(NetworkAddress localAddress);
	void stop();
	//CommsSignature localSignature();

	NetworkAddress localAddress();
	QString localID();

	QString getSummary();
	void setID(const QString &id);
	void setHookCommsSignals(QObject &ob, bool hook);
	void setHookCourierSignals(Courier &courier, bool hook);
	void setCourierRegistered(Courier &, bool);
	int courierCount();
	bool hasCourier(Courier &c) const;
	Courier *getCourierByID(quint32 id) const;
	bool isStarted() const;
	bool isConnected() const;

	qint64 sendRawData(QByteArray datagram, NetworkAddress address);

public slots:
	quint64 rescheduleSending(quint64 now);

private:

	void appendLog(QString);
	void sendData(const quint64 &now, QSharedPointer<CommsSession> localClient, Courier *courier);

	// CommsChannel signals
signals:
	//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
	void commsError(QString message);
	void commsClientAdded(CommsSession *c);
	void commsConnectionStatusChanged(bool c);

private slots:

	void receivePacketRaw(QByteArray ba,QHostAddress host, quint16 port);
	void onSendingTimer();

public slots:

	void onReadyRead();
	void onUdpError(QAbstractSocket::SocketError);
};



#endif // COMMSCHANNEL_HPP
