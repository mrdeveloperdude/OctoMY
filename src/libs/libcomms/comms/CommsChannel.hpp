/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef COMMSCHANNEL_HPP
#define COMMSCHANNEL_HPP


#include "uptime/SharedPointerWrapper.hpp"
#include "app/log/LogDestination.hpp"
#include "security/Key.hpp"


#include "utility/time/RateCalculator.hpp"
#include "CommsSessionDirectory.hpp"
#include "carriers/CommsCarrier.hpp"
#include "couriers/Courier.hpp"
#include "couriers/CourierSet.hpp"


#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QSet>


#define OCTOMY_PROTOCOL_MAGIC (0x0C701111)
//#define OCTOMY_PROTOCOL_MAGIC_IDLE (OCTOMY_PROTOCOL_MAGIC+1)
#define OCTOMY_PROTOCOL_VERSION_CURRENT (1)
#define OCTOMY_PROTOCOL_DATASTREAM_VERSION_CURRENT (QDataStream::Qt_5_7)

/*
 * Documentation is here: https://octomy.org/documentation/architecture/communication
*/

class HubWindow;
class CommsSession;
class CommsSessionDirectory;
class KeyStore;
class AddressList;
struct PacketReadState;
struct PacketSendState;
class AddressBook;


class CommsChannel : public QObject
{
	Q_OBJECT

protected:
	// The carrier such as udp or bluetooth used by this comms channel to communicate with the other side
	QSharedPointer<CommsCarrier> mCarrier;

	// The keystore which is used for encryption (the local key pair is used, as looked up with mLocalID)
	QSharedPointer<KeyStore> mKeystore;

	// The store which is used to find network addresses to use when creating new sessions
	QSharedPointer<AddressBook> mAssociates;

	// The directory of sessions for this cc
	CommsSessionDirectory mSessions;

	// The couriers that are in use by this cc. Only active couriers are in this list for performance reasons.
	CourierSet mCouriers;

	// Map for quickly finding a particular courier by its unique ID
	QMap<quint32, QSharedPointer<Courier> > mCouriersByID;

	quint64 mLocalSessionID;
	RateCalculator mTXScheduleRate;

	// When honeymoon mode is enabled, all inactive associates are pinged continuously in an effort to start new connections
	quint64 mHoneyMoonEnd;

	// Couriers with priority at next sending oportunity as calculated by rescheduleSending()
	QMap<quint64, QSharedPointer<Courier> > mSchedule;
	QSet<QString> mPendingHandshakes;

	// The number of milliseconds until next sending opportunity as calculated by rescheduleSending()
	qint64 mMostUrgentSendingTime;

	ConfigureHelper mConfigureHelper;

public:
	explicit CommsChannel(QObject *parent=nullptr);
	virtual ~CommsChannel();

public:
	void configure(QSharedPointer<CommsCarrier> carrier, QSharedPointer<KeyStore> keystore, QSharedPointer<AddressBook> peers);
	void activate(const bool on);

protected:
	//void detectConnectionChanges(const quint64 now);
	QSharedPointer<CommsSession>  createSession(QString id, bool initiator);
	QSharedPointer<CommsSession>  lookUpSession(QString id);


public:
	QSharedPointer<CommsCarrier> carrier();
	CommsSessionDirectory &sessions();
	NetworkAddress localAddress();
	QString localID();

	QString getSummary();
	//void setID(const QString &id);
	void hookCommsSignals(QObject &ob, bool hook);
	void hookCourierSignals(QSharedPointer<Courier>, bool hook);

	void registerCourier(QSharedPointer<Courier>, bool);
	CourierSet couriers();

	qint64 sendRawData(QByteArray datagram, NetworkAddress address);

	void setHoneymoonEnd(quint64 hEndMS);
	bool honeymoon(quint64 now=0);

	// Report if this CommsChannel would rather be connected or not (registered couriers > 0 and activated)
	bool needConnection();
	// [Dis]connect based on our needConnection()
	void updateConnection();


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
	// An error occurred in comms
	void commsError(QString message);
	// A new comms session was added
	void commsClientAdded(CommsSession *c);
	// The connection state changed for comms channel
	void commsConnectionStatusChanged(const bool isConnected, const bool needsConnection);

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


#endif
// COMMSCHANNEL_HPP
