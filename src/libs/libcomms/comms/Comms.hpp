#ifndef COMMS_HPP
#define COMMS_HPP

#include "CommsSessionDirectory.hpp"
#include "comms/couriers/CourierSet.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "utility/time/HoneymoonScheduler.hpp"

#include <QObject>

class AddressBook;
class Associate;
class CommsCarrier;
class CommsCarrierUDP;
class CommsCarrierBluetooth;
class CommsChannel;
class KeyStore;
class Courier;
class CommsSession;


#define FIRST_STATE_ID (static_cast<SESSION_ID_TYPE>(MULTIMAGIC_LAST))
#define MAX_CONCURRENT_COURIERS (100)


/**
 * @brief The Comms class is the top-level class for managing communications for
 * nodes. It manages one CommsChannel for each available CommsCarrier type.
 */
class Comms : public QObject, public QEnableSharedFromThis<Comms>
{
	Q_OBJECT
private:
	
	// The store which is used to find network addresses to use when creating new sessions
	QSharedPointer<AddressBook> mAssociates;
	
	// The directory of sessions for this cc
	CommsSessionDirectory mSessions;
	
	// The couriers that are in use by this cc. Only active couriers are in this list for performance reasons.
	CourierSet mCouriers;
	
	// The keystore which is used for encryption (the local key pair is used, as looked up with mLocalID)
	QSharedPointer<KeyStore> mKeystore;
	
	// When honeymoon mode is enabled, all inactive associates are pinged continuously in an effort to start new connections
	HoneymoonScheduler<qint64> mHoneymoon;
	
	// Map for quickly finding a particular courier by its unique ID
	QMap<quint32, QSharedPointer<Courier> > mCouriersByID;
	
	
	// Carriers
	QSharedPointer<CommsCarrierUDP> mCarrierUDP;
	QSharedPointer<CommsCarrierBluetooth> mCarrierBluetooth;

	// Channels
	QSharedPointer<CommsChannel> mChannelUDP;
	
	ConfigureHelper mConfigureHelper;
	
	
public:
	Comms(QObject *parent=nullptr);
	
public:
	void configure( QSharedPointer<KeyStore> keystore, QSharedPointer<AddressBook> peers);
	bool activate(bool on);
	void registerCourier(QSharedPointer<Courier>, bool);

	// Integration helpers
public:
	CommsSessionDirectory &sessions();
	QSharedPointer<KeyStore> keyStore() const;
	
//protected:
	QSharedPointer<CommsSession> createSession(const QString &id, const bool &initiator);
	QSharedPointer<CommsSession> sessionByAssociateID(const QString &id);
	QSharedPointer<CommsSession> sessionBySessionID(const SESSION_ID_TYPE &id);
	QSharedPointer<Associate> associateByID(const QString &id);
	QSharedPointer<Courier> courierByID(const quint32 &id);
	void triggerHoneymoon(const quint64 now=0);
	quint64 honeymoonValue(const quint64 now=0);
	
public:
	// Report if this CommsChannel would rather be connected or not (registered couriers > 0 and activated)
	bool needConnection();
	// [Dis]connect based on our needConnection()
	void updateConnection();
	
	CourierSet couriers();

public:
	bool audioOn() const;
	bool cameraOn() const;
	bool bluetoothOn() const;
	bool NFCOn() const;
	bool networkOn() const;
	bool zooOn() const;
	
private slots:
	void toggleAudio(const bool on);
	void toggleBluetooth(const bool on);
	void toggleNetwork(const bool on);
	void toggleZoo(const bool zoo);
	
	
	
	
	// CommsChannel signals (forwarded)
signals:
	// An error occurred in comms
	void commsError(QString message);
	// A new comms session was added
	void commsClientAdded(CommsSession *c);
	// The connection state changed for comms channel
	void commsConnectionStatusChanged(const bool isConnected, const bool needsConnection);
	
};

#endif // COMMS_HPP
