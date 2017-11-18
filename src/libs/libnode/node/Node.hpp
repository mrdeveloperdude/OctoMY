/* This file is distributed under the license as specified in the LICENSE file
* that accompanied it. If there was no LICENSE file accompanying this source,
* it is not to be modified, redistributed or used without prior written
* permission from original author and owner "Lennart Rolland".
*/

#ifndef NODE_HPP
#define NODE_HPP

#include "comms/CommsCarrierUDP.hpp"

#include "node/NodeRole.hpp"
#include "discovery/AddressBook.hpp"

#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"
#include "basic/LocalAddressList.hpp"
#include "ClientList.hpp"
#include "widgets/TryToggleState.hpp"
#include "comms/IConnectionStatus.hpp"


#include <QObject>
#include <QHostAddress>
#include <QUrl>

class CommsCarrier;
class CommsChannel;
class ZooClient;
class DiscoveryClient;


class SensorInput;
class CameraList;
class SensorsMessage;
class QCommandLineParser;

class QGeoPositionInfo;
class QCompassReading;
class QAccelerometerReading;
class QGyroscopeReading;
class CommsSession;
class AppContext;
class SensorsCourier;
class BlobCourier;
class BlobFuture;


class Node : public QObject, public IConnectionStatus
{
	Q_OBJECT
protected:

	AppContext *mContext;
	NodeRole mRole;
	NodeType mType;
	KeyStore mKeystore;
	// Client ids
	AddressBook mAddressBook;
	// Client instances
	ClientList mClients;
	// Our local network addresses
	LocalAddressList mAddresses;
	CommsCarrier *mCarrier;
	CommsChannel *mComms;
	DiscoveryClient *mDiscovery;
	ZooClient *mZooClient;
	SensorInput *mSensors;
	QSharedPointer<SensorsCourier> mSensorsCourier;
	QSharedPointer<BlobCourier> mBlobCourier;
	CameraList *mCameras;
	qint64 mLastStatusSend;
	QUrl mServerURL;

public:
	explicit Node(AppContext *context, NodeRole mRole, NodeType mType, QObject *parent = nullptr);
	virtual ~Node();

public:

	void init();
	void deInit();

	void updateDiscoveryClient();

	void setHookColorSignals(QObject &o, bool hook);
	void setHookSensorSignals(QObject &o, bool hook);
	void setHookCommsSignals(QObject &o, bool hook);
	void setHookPeerSignals(QObject &o, bool hook);


	// Selectors
public:
	NodeRole role();
	NodeType type();
	KeyStore &keyStore();
	AddressBook &addressBook();
	ClientList &clientList();
	LocalAddressList &localAddressList();
	QString name();
	CommsChannel *comms();
	DiscoveryClient *discoveryClient();
	ZooClient *zooClient();
	SensorInput *sensorInput();
	QSharedPointer<Associate> nodeIdentity();
	CameraList *cameraList();

	const QCommandLineParser &commandLine() const;
	Settings &settings();

	QSet<QSharedPointer<Associate> > allControls();
	QSet<QSharedPointer<Associate> > controlsWithActiveSessions(quint64 now=0);

	//bool courierRegistration();
	//void  rierRegistration(QSharedPointer<Associate>, bool);
	//void updateCourierRegistration(quint64 now=0);


	//TryToggleState updateOnlineStatus(const TryToggleState currentTryState);


	// Couriers

	// [Un]register local couriers with comms
	virtual void setNodeCouriersRegistered(const bool reg);
	// [Un]register client specific couriers with comms
	void setClientCouriersRegistered(const bool reg);
	// [Un]register couriers with comms
	void setCouriersRegistered(const bool reg);

	// Update client specific courier registration with comms depending on need
	void updateClientCouriersRegistered();
	// Update node specific courier registration with comms depending on need
	void updateNodeCouriersRegistered();
	// Update courier registration with comms depending on need
	void updateCouriersRegistered();



	// Actions
public:

	virtual QWidget *showWindow();

	bool isCommsStarted();
	bool isCommsConnected();


	// IConnectionStatus interface
public:
	bool needsConnection() Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;
	void setNeedsConnection(const bool) Q_DECL_OVERRIDE;
	void setConnected(const bool) Q_DECL_OVERRIDE;


	// Blob courier
	BlobFuture submitBlobForSending(QByteArray data, QString name);


	// Blob courier
signals:
	void blobReceived(QString name);
	void blobSendComplete(QString name);

	// KeyStore slots
private slots:
	void onKeystoreReady(bool);

	// CommsChannel slots
private slots:
	virtual void onCommsError(QString);
	virtual void onCommsClientAdded(CommsSession *);
	virtual void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);


	// SensorInput slots
private slots:
	void onPositionUpdated(const QGeoPositionInfo &info);
	void onCompassUpdated(QCompassReading *);
	void onAccelerometerUpdated(QAccelerometerReading *);
	void onGyroscopeUpdated(QGyroscopeReading *r);

};

#endif // NODE_HPP
