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

#include "security/Key.hpp"

#include "couriers/Courier.hpp"
#include "CommsSessionDirectory.hpp"


#include "CommsCarrier.hpp"

#include "Multimagic.hpp"

#include "RateCalculator.hpp"

#include "couriers/CourierSet.hpp"

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



class PacketReadState;
class PacketSendState;
class AddressBook;

class CommsChannel : public QObject
{
	Q_OBJECT

protected:

	CommsCarrier &mCarrier; // The carrier such as udp or bluetooth used by this cc to communicate with the other side
	KeyStore &mKeystore;   // The keystore which is used for encryption (the local key pair is used, as looked up with mLocalID)
	AddressBook &mAssociates; // The store wich is used to find network addresses to use when creating new sessions
	CommsSessionDirectory mSessions; // The directory of sessions for this cc
	CourierSet mCouriers; // The couriers that are in use by this cc. Only active couriers are in this list for performance reasons.
	QMap<quint32, Courier *> mCouriersByID; // Map for quickly finding a particular courier by it's unique ID
	quint64 mLocalSessionID;
	RateCalculator mTXScheduleRate;

	// When honeymoon mode is enabled, all inactive associates are pinged continuously in an effort to start new connections
	quint64 mHoneyMoonEnd;

	QMap<quint64, Courier *> mSchedule; // Couriers with priority at next sending oportunity as calculated by rescheduleSending()
	QSet<QString> mPendingHandshakes;
	qint64 mMostUrgentSendingTime; // The number of milliseconds until next sending opportunity as calculated by rescheduleSending()

public:

	explicit CommsChannel(CommsCarrier &carrier, KeyStore &keystore, AddressBook &peers, QObject *parent=nullptr);
	//TODO: Remove once nobody refers to it any more
	//explicit CommsChannel(CommsCarrier &carrier, KeyStore &keystore, QObject *parent=nullptr);
	virtual ~CommsChannel();

protected:

	//void detectConnectionChanges(const quint64 now);
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
	bool courierRegistration();
	CourierSet couriers();
	int courierCount();
	bool hasCourier(Courier &c) const;
	Courier *getCourierByID(quint32 id) const;

	qint64 sendRawData(QByteArray datagram, NetworkAddress address);

	void setHoneymoonEnd(quint64 hEndMS);
	bool honeymoon(quint64 now=0);

protected:

	void appendLog(QString);
	void doSendWithSession( PacketSendState &state);
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
	void commsConnectionStatusChanged(const bool c);

public slots:
	// Re-calculate the schedule for sending timer. May result in sending timer being called at a different time then what it was scheduled for when this method was called
	// You should call this whenever there is a chance the schedule might have changed
	quint64 rescheduleSending(quint64 now);


protected:
	bool recieveEncryptedBody(PacketReadState &state);
	bool recieveMagicAndVersion(PacketReadState &state);

	void recieveIdle(PacketReadState &state);

	void recieveHandshake(PacketReadState &state);
	bool recieveSyn(PacketReadState &state, QSharedPointer<CommsSession> session);
	bool recieveSynAck(PacketReadState &state, QSharedPointer<CommsSession> session);
	bool recieveAck(PacketReadState &state, QSharedPointer<CommsSession> session);

	void recieveData(PacketReadState &state);

// Send & receive slots
protected slots:
	void receivePacketRaw(QByteArray ba,QHostAddress host, quint16 port);

	// Carrier slots
public slots:
	void onCarrierReadyRead();
	void onCarrierError(const QString error);
	void onCarrierSendingOpportunity(const quint64 now);
	void onCarrierConnectionStatusChanged(const bool connected);

};



#endif // COMMSCHANNEL_HPP
