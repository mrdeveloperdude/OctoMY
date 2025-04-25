/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef COMMSCHANNEL_HPP
#define COMMSCHANNEL_HPP


#include "CommsSessionDirectory.hpp"
#include "ProtocolMagic.hpp"
#include "address/Associate.hpp"
#include "carriers/CommsCarrier.hpp"
#include "couriers/Courier.hpp"
#include "couriers/CourierSet.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "utility/time/RateCalculator.hpp"


#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QSet>


class AddressBook;
class AddressList;
class Comms;
class CommsSession;
class CommsSessionDirectory;
class HubWindow;
class Key;
class KeyStore;
struct PacketReadState;
struct PacketSendState;


struct SendingSchedule{
	// Couriers with priority at next sending opportunity as calculated by rescheduleSending()
	QMap<quint64, QSharedPointer<Courier> > schedule;
	// The number of milliseconds until next sending opportunity as calculated by rescheduleSending()
	qint64 mMostUrgentSendingTime;
	
};

/**
 * @brief Documentation is here: https://octomy.org/documentation/architecture/communication
*/
class CommsChannel : public QObject
{
	Q_OBJECT

protected:
	// The Comms that this channel belongs to
	QSharedPointer<Comms> mComms;
	
	// The carrier such as udp or bluetooth used by this comms channel to communicate with the other side
	QSharedPointer<CommsCarrier> carrier;
	
	// The currently calculated sendin g schedule
	SendingSchedule mSendingSchedule;
	

	quint64 mLocalSessionID;
	
	RateCalculator mTXScheduleRate;

	

	QSet<QString> mPendingHandshakes;


	ConfigureHelper mConfigureHelper;

public:
	explicit CommsChannel(QObject *parent=nullptr);
	virtual ~CommsChannel();

public:
	void configure(QSharedPointer<Comms> comms, QSharedPointer<CommsCarrier> carrier);
	void activate(const bool on);


public:
	QSharedPointer<CommsCarrier> getCarrier();
	
	QSharedPointer<CarrierAddress> localAddress();
	QString localID();

	QString getSummary();
	void hookCommsSignals(QObject &ob, bool hook);
	void hookCourierSignals(QSharedPointer<Courier>, bool hook);

	void registerCourier(QSharedPointer<Courier>, bool);
	CourierSet couriers();
	void triggerHoneymoon(const quint64 now=0);
	quint64 honeymoonValue(const quint64 now=0);

	qint64 sendRawData(QByteArray datagram, QSharedPointer<CarrierAddress> address);

	// Report if this CommsChannel would rather be connected or not (registered couriers > 0 and activated)
	bool needConnection();
	// [Dis]connect based on our needConnection()
	void updateConnection();
	
	
	// Transition to Comms helper
public:
	QSharedPointer<KeyStore> keyStore() const;
	QSharedPointer<Key> localKey() const;
	CommsSessionDirectory &sessions();
	QSharedPointer<CommsSession> createSession(QString id, bool initiator);
	QSharedPointer<CommsSession> sessionByAssociateID(QString id);
	QSharedPointer<CommsSession> sessionBySessionID(SESSION_ID_TYPE id);
	QSharedPointer<Associate> associateByID(const QString &id);
	QSharedPointer<Courier> courierByID(const quint32 &id);

protected:
	void appendLog(const QString &text);
	void doSendWithSession( PacketSendState &state);
	void sendHandshake(const quint64 &now, const QString handShakeID);
	void sendSyn(PacketSendState &state);
	void sendSynAck(PacketSendState &state);
	void sendAck(PacketSendState &state);
	
	// Idle packets are there just to keep the connection from collapsing. This is necessary for ceratin carriers such as UDP
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
	// Re-calculate the schedule for sending timer. May result in sending timer being called at a different time than what it was scheduled for when this method was called
	// You should call this whenever there is a chance the schedule might have changed
	quint64 rescheduleSending(quint64 now);
	
	
protected:
	bool recieveEncryptedBody(PacketReadState &state);
	bool recieveMagicAndVersion(PacketReadState &state);
	// Idle packets are there just to keep the connection from collapsing. This is necessary for ceratin carriers such as UDP
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
