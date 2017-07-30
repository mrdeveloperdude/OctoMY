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
#include <QSharedPointer>
#include <QSet>

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"

#include "../../libcore/basic/LogDestination.hpp"
#include "couriers/Courier.hpp"
#include "CommsSessionDirectory.hpp"

// It seems that 30 seconds would be a "safe" minimal UDP rate to avoid routers closing our "connection"
#define MAXIMAL_PACKET_RATE (1000)
#define MINIMAL_PACKET_RATE (5000)

#define OCTOMY_PROTOCOL_MAGIC (0x0C701111)
//#define OCTOMY_PROTOCOL_MAGIC_IDLE (OCTOMY_PROTOCOL_MAGIC+1)
#define OCTOMY_PROTOCOL_VERSION_CURRENT (1)
#define OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT (QDataStream::Qt_5_7)



class HubWindow;
class CommsSession;
class CommsSessionDirectory;
class KeyStore;

/*
 * Documentation moved here: https://sites.google.com/site/octomyproject/documentation/development/architectual-overview/communications
*/

enum Multimagic:SESSION_ID_TYPE {
	MULTIMAGIC_IDLE=0
	, MULTIMAGIC_SYN
	, MULTIMAGIC_SYNACK
	, MULTIMAGIC_ACK
	, MULTIMAGIC_LAST

};

QString MultimagicToString(SESSION_ID_TYPE m);

QString MultimagicToString(Multimagic m);


#include "RateCalculator.hpp"

class PacketReadState;
class PacketSendState;
class NodeAssociateStore;

class CommsChannel : public QObject
{
	Q_OBJECT
private:
	static const quint64 CONNECTION_TIMEOUT;
	static const qint32 MAX_UDP_PAYLOAD_SIZE;

	//Receive counter used in debug messages to make sense of the order of things
	static quint32 sTotalRecCount;
	static quint32 sTotalTxCount;

private:

	QUdpSocket mUDPSocket; // The socket used by this cc to communicate with the other side
	QTimer mSendingTimer;  // The timer used to schedule when packets are sent to the other side
	KeyStore &mKeystore;   // The keystore which is used for encryption (the local key pair is used, as looked up with mLocalID)
	NodeAssociateStore &mPeers; // The store wich is used to find network addresses to use when creating new sessions
	CommsSessionDirectory mSessions; // The directory of sessions for this cc
	QList<Courier *> mCouriers; // The couriers that are in use by this cc. Only active couriers are in this list for performance reasons.
	QMap<quint32, Courier *> mCouriersByID; // Map for quickly finding a particular courier by it's unique ID
	quint64 mLocalSessionID;
	NetworkAddress mLocalAddress;

	RateCalculator mRXRate;
	RateCalculator mTXRate;
	RateCalculator mTXOpportunityRate;
	RateCalculator mTXScheduleRate;

	bool mConnected;
	// When honeymoon mode is enabled, all inactive associates are pinged continuously in an effort to start new connections
	bool mHoneyMoon;

	QMap<quint64, Courier *> mSchedule; // Couriers with priority at next sending oportunity as calculated by rescheduleSending()
	QSet<QString> mPendingHandshakes;
	qint64 mMostUrgentSendingTime; // The number of milliseconds until next sending opportunity as calculated by rescheduleSending()

public:

	explicit CommsChannel(KeyStore &keystore, NodeAssociateStore &peers, QObject *parent=nullptr);
	explicit CommsChannel(KeyStore &keystore, QObject *parent=nullptr);

private:

	void detectConnectionChanges(const quint64 now);
	QSharedPointer<CommsSession>  createSession(QString id, bool initiator);
	QSharedPointer<CommsSession>  lookUpSession(QString id);


public:

	CommsSessionDirectory &sessions();
	void start(NetworkAddress localAddress);
	void stop();
	bool isStarted() const;
	bool isConnected() const;

	NetworkAddress localAddress();
	QString localID();

	QString getSummary();
	//void setID(const QString &id);
	void setHookCommsSignals(QObject &ob, bool hook);
	void setHookCourierSignals(Courier &courier, bool hook);
	void setCourierRegistered(Courier &, bool);
	int courierCount();
	bool hasCourier(Courier &c) const;
	Courier *getCourierByID(quint32 id) const;

	qint64 sendRawData(QByteArray datagram, NetworkAddress address);

private:

	void appendLog(QString);
	void doSend( PacketSendState &state);
	void sendHandshake(const quint64 &now, const QString handShakeID);
	void sendSyn(PacketSendState &state);
	void sendSynAck(PacketSendState &state);
	void sendAck(PacketSendState &state);

	void sendIdle(const quint64 &now, QSharedPointer<CommsSession> session);
	void sendCourierData(const quint64 &now, Courier &courier);

	// CommsChannel signals
signals:
	//		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
	void commsError(QString message);
	void commsClientAdded(CommsSession *c);
	void commsConnectionStatusChanged(bool c);

public slots:
	// Re-calculate the schedule for sending timer. May result in sending timer being called at a different time then what it was scheduled for when this method was called
	// You should call this whenever there is a chance the schedule might have changed
	quint64 rescheduleSending(quint64 now);


private:

	bool recieveEncryptedBody(PacketReadState &state);
	bool recieveMagicAndVersion(PacketReadState &state);

	void recieveIdle(PacketReadState &state);
	void recieveSyn(PacketReadState &state);
	void recieveSynAck(PacketReadState &state);
	void recieveAck(PacketReadState &state);
	void recieveData(PacketReadState &state);

// Send & receive slots
private slots:

	void receivePacketRaw(QByteArray ba,QHostAddress host, quint16 port);
	void onSendingTimer();

public slots:

	void onReadyRead();
	void onUdpError(QAbstractSocket::SocketError);
};



#endif // COMMSCHANNEL_HPP
