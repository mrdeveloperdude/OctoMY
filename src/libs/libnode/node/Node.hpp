/* This file is distributed under the license as specified in the LICENSE file
* that accompanied it. If there was no LICENSE file accompanying this source,
* it is not to be modified, redistributed or used without prior written
* permission from original author and owner "Lennart Rolland".
*/

#ifndef NODE_HPP
#define NODE_HPP

#include "address/LocalAddressList.hpp"
#include "app/Settings.hpp"
#include "client/ClientList.hpp"

#include "log/LogType.hpp"
#include "node/NodeRole.hpp"
#include "node/NodeType.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QObject>
#include <QHostAddress>
#include <QUrl>

class LogStorage;

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
class AddressBook;
class KeyStore;
class PlanBook;
class LocalIdentityStore;

class IAppLauncher;
class Associate;
class ScopedTimer;

class ServiceLevelManager;
class ServiceLevel;
class KeyStoreService;
class LocalIdentityStoreService;
class LocalAddressListService;
class AddressBookService;
class PlanBookService;
class DiscoveryClientService;
class CarrierService;
class CommsService;
class Comms;

class NodeWindow;

/*!
 * \brief The Node class is the base class for agents, remotes and hubs.
 *
 * It holds all functionality that is common among them, including mechanisms
 * for discovering, pairing, communicating with other nodes, storage of vital
 * state such as the identity of associates, communication with OctoMY zoo and
 * more
 *
 */

class Node : public QObject/*, public IConnectionStatus*/, public QEnableSharedFromThis<Node>
{
	Q_OBJECT

private:
	// The application launcher that launched us
	QSharedPointer<IAppLauncher> mLauncher;
	// Helper to keep track of appConfigure() and appActivate() state
	ConfigureHelper mAppConfigureHelper;
	// The storage for application logs
	QSharedPointer<LogStorage> mLogStorage;
	// Our key store
	QSharedPointer<KeyStore> mKeyStore;
	// Our local identity storage
	QSharedPointer<LocalIdentityStore> mLocalIdentityStore;
	// Our local network addresses
	QSharedPointer<LocalAddressList> mLocalAddressList;
	// Clients address book (clients are nodes we communicate with)
	QSharedPointer<AddressBook> mAddressBook;
	// Plan book (centralized plans management)
	QSharedPointer<PlanBook> mPlanBook;
	// Client instances
	QSharedPointer<ClientList> mClients;
	// Our identity
	QSharedPointer<Associate> mNodeIdentity;
	// Discovery service to automatically discover other nodes
	QSharedPointer<DiscoveryClient> mDiscovery;
	// Comms handler for managing all communications for this node
	QSharedPointer<Comms> mComms;
	// Comms channel carrier the underlying carrier such as udb/bluetooth/eth for comms
	//QSharedPointer<CommsCarrier> mCarrier;
	// Comms channel used to communicate with other nodes
	//QSharedPointer<CommsChannel> mComms;
	// The zoo client is used to exchange data via the central repository server (zoo)
	QSharedPointer<ZooClient> mZooClient;
	// Helper to access all sensors available via Qt's sensor API
	QSharedPointer<SensorInput> mSensors;
	// Comms Courier to manage transmission of changes in sensor data
	QSharedPointer<SensorsCourier> mSensorsCourier;
	// Comms Courier to manage transmission of blobs  such as images, files and video streams (they are treated as blobs)
	QSharedPointer<BlobCourier> mBlobCourier;
	// Camera list manages attached cameras adapting to dis- and re- connections
	QSharedPointer<CameraList> mCameras;
	// Unix ms timestamp of last sending time of status
	qint64 mLastStatusSend;
	// The URL of the zoo server
	QUrl mServerURL;
	// Timer used to log how long activation takes from start to finish
	ScopedTimer *mActivationTimer;


	// Manager for maintaining the lifecycle and interdependencies of services
	QSharedPointer<ServiceLevelManager> mServiceLevelManager;
	// Service wrapper for KeyStore
	QSharedPointer<KeyStoreService> mKeyStoreService;
	// Service wrapper for LocalIdentityStore
	QSharedPointer<LocalIdentityStoreService> mLocalIdentityStoreService;
	// Service wrapper for LocalAddressList
	QSharedPointer<LocalAddressListService> mLocalAddressListService;
	// Service wrapper for AddressBook
	QSharedPointer<AddressBookService> mAddressBookService;
	// Service wrapper for PlanBook
	QSharedPointer<PlanBookService> mPlanBookService;
	// Service wrapper for Discovery
	QSharedPointer<DiscoveryClientService> mDiscoveryService;

	// Service wrapper for Comms Carrier
	QSharedPointer<CarrierService> mCarrierService;
	// Service wrapper for Comms
	QSharedPointer<CommsService> mCommsService;

	// Service Level that will always be active as long as node is running
	QSharedPointer<ServiceLevel> mAlwaysServiceLevel;

	// Service Level that will be active as long as node is in discovery mode
	QSharedPointer<ServiceLevel> mDiscoveryServiceLevel;
	
	// Output debug log
	bool mDebug{false};
	
	// The current state of panic mode. If enabled the node will engage safety measures such as limping all actuators and blinking warning lights
	bool mPanic;


public:
	explicit Node();
	virtual ~Node() override;

	// AppLauncher interface
public:

	// Called by launcher to give the app a chance to allocate data before initialization
	virtual void appConfigure(QSharedPointer<IAppLauncher> launcher);

	// Called with on=true by launcher to give the app a chance to initialize
	// After calling this with on=true, the launcher will pause further initialization and showing of windows until app emits an "appInitDone" signal

	// Called with on=false by launcher as a response to an appRequestClose event being emitted by anyone that wants the app to close.
	// After calling this with on=false, the launcher will complete termination and return to OS
	virtual void appActivate(const bool on);

	// Called by launcher to get a handle to the app's main window
	// Will be called when launcher wants to show window during initialization
	// NOTE: Launcher will NOT hide the window, as it is expected that app should do this by itself upon de-initialization and/or destruction
	virtual QSharedPointer<NodeWindow> appWindow();

	// Node interface
public:
	// Called by Node::appConfigure();
	virtual void nodeConfigure()=0;

	// Called by Node::appActivate()
	virtual void nodeActivate(const bool on)=0;

	// Called by Node::appWindow()
	virtual QSharedPointer<NodeWindow> nodeWindow()=0;

	// Provide role
	virtual NodeRole nodeRole()=0;

	// Provide type
	virtual NodeType nodeType()=0;


public:
	QSharedPointer<LogStorage> logStorage() const;
	void log(const QString &text, LogType type=LOG_TYPE_INFO) const;

	// Selectors
public:

	// Provide launcher instance
	QSharedPointer<IAppLauncher> launcher();

	// Provide current context (adapter for context in app launcher instance)
	QSharedPointer<AppContext> context();

	// Provide the key store
	QSharedPointer<KeyStore> keyStore();

	// Provide the local address list (network addresses)
	QSharedPointer<LocalAddressList> localAddressList();

	// Convenience wrapper to get context's settings
	QSharedPointer<Settings> settings();
	
	// Provide the address book
	QSharedPointer<AddressBook> addressBook();
	
	// Provide the plan book
	QSharedPointer<PlanBook> planBook();
	
	//Provide the client list
	QSharedPointer<ClientList> clientList();
	
	// Provide the comms handler
	QSharedPointer<Comms> comms();
	
	// Provide the discovery client
	QSharedPointer<DiscoveryClient> discoveryClient();
	
	// Provide the zoo client
	QSharedPointer<ZooClient> zooClient();
	
	// Provide the sensor input
	QSharedPointer<SensorInput> sensorInput();
	
	// Provide the node identity
	QSharedPointer<Associate> nodeIdentity();
	
	// Provide the service level manager
	QSharedPointer<ServiceLevelManager> serviceLevelManager();
	
	// Provide the URL of the Zoo server
	QUrl serverURL();
	
	// This is internal to nodes
protected:
	// Provide local identity. Only for internal use, use nodeIdentity() as a user
	QSharedPointer<LocalIdentityStore> localIdentityStore();
	
	
	
	
public:
	// Accept triggering or un-triggering of panic state
	void setPanic(bool panic);
	// Get panic state
	bool panic();
	

	// Actions
public:

	// Reset the identity and settings for this node. WARNING: This is destructive and mostly useful when debugging
	void unbirth();
	
	// Reset the identity and settings for this node forcefully, deleting files etc. WARNING: This is destructive and mostly useful when debugging
	void scrape();

	// Set the OctoMY™ styling to the UI of the application
	void applyAppStyle();
	
	// Set the OctoMY™ styling to the UI of the node window
	void applyWindowStyle();

	// Create the dir in which this node keeps its data
	bool createBaseDir();
	
	// Toggle discovery active
	void discoveryActivate(const bool on);

	// Below this line is unrefined
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////


public:
	void setHookColorSignals(QObject &o, bool hook);
	void setHookSensorSignals(QObject &o, bool hook);
	void setHookCommsSignals(QObject &o, bool hook);

	// Convenience wrapper for setNodeIdentity( QSharedPointer<Associate> nodeID)
	void setNodeIdentity(QVariantMap map);
	// Invoked by Delivery wizards when we are born, and by LocalIdentityStore on startup as soon as our identity is ready
	void setNodeIdentity(QSharedPointer<Associate> nodeID);
	CameraList *cameraList();

	//QSet<QSharedPointer<Associate> > allControls();
	//QSet<QSharedPointer<Associate> > controlsWithActiveSessions(quint64 now=0);

	//bool courierRegistration();
	//void  rierRegistration(QSharedPointer<Associate>, bool);
	//void updateCourierRegistration(quint64 now=0);



	//TryToggleState updateOnlineStatus(const TryToggleState currentTryState);




	virtual QSharedPointer<Node> sharedThis();
	//QSharedPointer<Node> sharedThisNode();

signals:
	void identityChanged();

	// Couriers
public:
	// [Un]register local couriers with comms
	virtual void registerNodeCouriers(const bool reg);
	// [Un]register client specific couriers with comms
	void registerClientCouriers(const bool reg);
	// [Un]register both local and client couriers with comms
	void registerCouriers(const bool reg);

	// Update client specific courier registration with comms depending on need
	void updateClientCouriersRegistration();
	// Update node specific courier registration with comms depending on need
	void updateNodeCouriersRegistration();
	// Update courier registration with comms depending on need
	void updateCouriersRegistration();


	/*
		// Actions
	public:
		bool isCommsStarted();
		bool isCommsConnected();
	*/

	// IConnectionStatus interface
public:
	// We want to go online. We might not be online and we might
	//bool needsConnection() ;
	//void setNeedsConnection(const bool) ;
	// We are currently online. We might not need to be online
	//bool isConnected() override;
	//void setConnected(const bool) override;

	// Blob courier
	BlobFuture submitBlobForSending(QByteArray data, QString name);


	// Blob courier
signals:
	void blobReceived(QString name);
	void blobSendComplete(QString name);

	/*
	// KeyStore slots
	private slots:
	void onKeystoreReady(bool);
	*/

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

signals:
	// Expected to be emitted when someone wants node to terminate.
	// Will be caught by app launcher and app launcher will subsequently call appActivate(false) to bring termination to fruition
	void nodeRequestExit(const int returnValue);

	// Expected to be emitted when someone wants node to restart.
	// Will be caught by app launcher and app launcher will subsequently call appActivate(false) to bring termination to fruition
	// before starting the process again afresh
	// TODO Implement
	void nodeRequestRestart();

	// Expected to be emitted with on=true by node as soon as initialization is done in appActivate(true);
	// Expected to be emitted with on=false by node during termination as soon as it no longer needs the eventloop in appActivate(false).
	void nodeActivateChanged(const bool on);

};

typedef Node Nodee;


#endif
// NODE_HPP
