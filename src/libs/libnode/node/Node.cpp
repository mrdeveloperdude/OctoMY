/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "Node.hpp"

#include "NodeWindow.hpp"
#include "address/Associate.hpp"
#include "app/AppContext.hpp"
#include "app/Constants.hpp"
#include "app/launcher/IAppLauncher.hpp"
#include "app/style/StyleManager.hpp"
#include "client/ClientList.hpp"
#include "client/zoo/ZooClient.hpp"
#include "comms/Comms.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "comms/carriers/CommsCarrierUDP.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"
#include "delivery/BirthControl.hpp"
#include "discovery/AddressBook.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "hardware/sensors/SensorInput.hpp"
#include "log/LogStorage.hpp"
#include "node/LocalIdentityStore.hpp"
#include "security/KeyStore.hpp"
#include "service/ServiceLevel.hpp"
#include "service/ServiceLevelManager.hpp"
#include "service/services/AddressBookService.hpp"
#include "service/services/CarrierService.hpp"
#include "service/services/CommsService.hpp"
#include "service/services/DiscoveryClientService.hpp"
#include "service/services/KeyStoreService.hpp"
#include "service/services/LocalAddressListService.hpp"
#include "service/services/LocalIdentityStoreService.hpp"
#include "service/services/PlanBookService.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/time/ScopedTimer.hpp"

#include <QAccelerometerReading>
#include <QCommandLineParser>
#include <QDir>
#include <QGeoPositionInfo>
#include <QGyroscopeReading>
#include <QStandardPaths>


static const QString NODE_ONLINE_STATUS_BASE_KEY("octomy.node.online.");



Node::Node()
	: QObject(nullptr)
	, mAppConfigureHelper("node", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	, mLogStorage(OC_NEW LogStorage())
	, mKeyStore(OC_NEW KeyStore())
	, mLocalIdentityStore(OC_NEW LocalIdentityStore())
	, mLocalAddressList(OC_NEW LocalAddressList())
	, mAddressBook(OC_NEW AddressBook())
	, mPlanBook(OC_NEW PlanBook())
	, mClients(OC_NEW ClientList())
	, mDiscovery(OC_NEW DiscoveryClient())
	//, mCarrier(OC_NEW CommsCarrierUDP())
	//, mComms(OC_NEW CommsChannel())
	, mComms(OC_NEW Comms())
	, mLastStatusSend (0)
	, mServerURL(QString("http://zoo.octomy.org:%1/api").arg(Constants::OCTOMY_UDP_DEFAULT_PORT_ZOO)) //pointed to localhost using /etc/hosts
	, mActivationTimer(nullptr)
	, mServiceLevelManager(OC_NEW ServiceLevelManager())
	, mKeyStoreService(OC_NEW KeyStoreService(mKeyStore, QStringList{}))
	, mLocalIdentityStoreService(OC_NEW LocalIdentityStoreService(mLocalIdentityStore, QStringList{mKeyStoreService->name()}))
	, mLocalAddressListService(OC_NEW LocalAddressListService(mLocalAddressList, QStringList{mLocalIdentityStoreService->name()}))
	, mAddressBookService(OC_NEW AddressBookService(mAddressBook, QStringList{mLocalIdentityStoreService->name()}))
	, mPlanBookService(OC_NEW PlanBookService(mPlanBook, QStringList{mLocalIdentityStoreService->name()}))
	, mDiscoveryService(OC_NEW DiscoveryClientService(mDiscovery, QStringList{mLocalIdentityStoreService->name()}))
//	, mCarrierService(OC_NEW CarrierService(mCarrier, QStringList{mLocalIdentityStoreService->name()}))
	, mCommsService(OC_NEW CommsService(mComms, QStringList{mKeyStoreService->name(), mAddressBookService->name()}))
	, mAlwaysServiceLevel(OC_NEW ServiceLevel("Always",
{
												  mKeyStoreService->name(), mLocalIdentityStoreService->name(), mLocalAddressListService->name(), mAddressBookService->name(), mPlanBookService->name(), mCommsService->name()
}))
, mDiscoveryServiceLevel(OC_NEW ServiceLevel("Discovery", {
	mDiscoveryService->name()
}))
{
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"Node()";
	}
}


Node::~Node()
{
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"~Node()";
	}
}


// TODO: Look at simplifying init system by combining appInit and appConfigure
//  , renaming appDeInit to appDeConfigure, and maybe removing one layer so there
//	is not appXXX + nodeXXX but just one of them.
void Node::appConfigure(QSharedPointer<IAppLauncher> launcher)
{
	OC_METHODGATE();
	if(mAppConfigureHelper.configure()) {
		if(mDebug){
			qDebug()<<"appConfigure()";
		}
		mLauncher = launcher;
		auto ctx = context();
		if(!ctx.isNull()) {
			ScopedTimer nodeConfigureTimer(context()->base() + "-configure", mDebug);
			setObjectName(ctx->base());
			applyAppStyle();
			const bool basedirOK = createBaseDir();
			if(basedirOK) {
				auto baseDir = ctx->baseDir();

				// Configure all services
				//////////////////////////////////
				
				mLogStorage->configure(baseDir + "/logs");
				
				mKeyStore->configure(baseDir + "/keystore.json", false);
				mKeyStoreService->configure();

				mLocalIdentityStore->configure(baseDir + "/local_identity.json");
				mLocalIdentityStoreService->configure(sharedThis());

				mLocalAddressList->configure(defaultPortForNodeType(nodeType()));
				mLocalAddressListService->configure();

				mAddressBook->configure(baseDir + "/addressbook.json");
				mAddressBookService->configure(sharedThis());
				
				mPlanBook->configure(baseDir + "/planbook.json");
				mPlanBookService->configure(sharedThis());

				mDiscovery->configure(sharedThis(), 1000, 60000, 20000, 40000);
				mDiscoveryService->configure(sharedThis());

				//mCarrier->configure();
				//mCarrierService->configure(sharedThis());

				mComms->configure(mKeyStore, mAddressBook);
				mCommsService->configure();


				// Register services with the service manager
				/////////////////////////////////////////////////////

				mServiceLevelManager->configure();

				mServiceLevelManager->registerService(mKeyStoreService);
				mServiceLevelManager->registerService(mLocalIdentityStoreService);
				mServiceLevelManager->registerService(mLocalAddressListService);
				mServiceLevelManager->registerService(mAddressBookService);
				mServiceLevelManager->registerService(mPlanBookService);
				mServiceLevelManager->registerService(mDiscoveryService);
				//mServiceLevelManager->registerService(mCarrierService);
				//mServiceLevelManager->registerService(mCommsService);

				// Register the "Always" service level
				//////////////////////////////////////
				mServiceLevelManager->registerServiceLevel(mAlwaysServiceLevel);


				// Register the "Discovery" service level
				//////////////////////////////////////
				mServiceLevelManager->registerServiceLevel(mDiscoveryServiceLevel);

				// TODO: It is not entirely clear what zoo client provides beyond that which discovery client provides. Until that has been resolved, we avoid it
				//mZooClient->configure();

				// TODO: This is the good stuff. It will be handled as soon as the boring stuff is finished and working xD
				//mSensors->configure(OC_NEW SensorInput(this))
				//mSensorsCourier->configure(QSharedPointer<SensorsCourier>::create(*mComms, this))
				//mBlobCourier->configure(QSharedPointer<BlobCourier>::create(*mComms, this))
				//mCameras->configure(OC_NEW CameraList(this))
				nodeConfigure();

			} else {
				mAppConfigureHelper.configureFailed("Could not create base dir");
			}
		} else {
			qWarning()<<"ERROR: No context";
		}
	}
}


void Node::appActivate(const bool on)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "EUREKA appActivate called: " << on;
	}
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		mServiceLevelManager->enableLevel(mAlwaysServiceLevel->name(), on, [=](bool ok) {
			if(mDebug){
				qDebug() << "EUREKA appActivate returned: " << (on?"ON":"OFF") << ok;
			}
			if(on){
				setHookSensorSignals(*this, on);
				//setHookSensorSignals(*mSensorsCourier, true);
				setHookCommsSignals(*this, on);
				//if(nullptr!=mZooClient) {
				//	mZooClient->setURL(mServerURL);
				//}
				// Bootstrap connection
				//const bool last=needsConnection();
				//setNeedsConnection(false);
				//setNeedsConnection(last);
				BirthControl bc;
				bc.configure(sharedThis());
				auto asessment = bc.statusAssesment();
				if(mDebug){
					qDebug() << asessment.toString();
				}
				if(ok){
					// TODO: Heed the OK status
				}
			}
			// NOTE: It is expected that this emits nodeActivateChanged() at some point
			nodeActivate(on);
			if(!on){
				setHookCommsSignals(*this, on);
				setHookSensorSignals(*this, on);
				if(ok){
					
				}
			}
			if(mDebug){
				qDebug() << "EUREKA ServiceActivation returned: " << ok;
			}
		});
	}
}


QSharedPointer<NodeWindow> Node::appWindow()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug()<<"appWindow()";
		}
		return nodeWindow();
	}
	return nullptr;
}


QSharedPointer<LogStorage> Node::logStorage() const{
	OC_METHODGATE();
	return mLogStorage;
}


void Node::log(const QString &text, LogType type) const{
	OC_METHODGATE();
	if(mLogStorage){
		mLogStorage->appendLog(text);
	}
	else{
		qDebug() << type << ": " << text;
	}
}

QSharedPointer<IAppLauncher> Node::launcher()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mLauncher;
	}
	return nullptr;
}


QSharedPointer<AppContext> Node::context()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mLauncher.isNull()?nullptr:mLauncher->context();
	}
	return nullptr;
}


QSharedPointer<KeyStore> Node::keyStore()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mKeyStore;
	}
	return nullptr;
}


QSharedPointer<LocalAddressList> Node::localAddressList()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mLocalAddressList;
	}
	return nullptr;
}


QSharedPointer<Settings> Node::settings()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		auto c=this->context();
		return c.isNull()?nullptr:c->settings();
	}
	return nullptr;
}


QSharedPointer<LocalIdentityStore> Node::localIdentityStore()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mLocalIdentityStore;
	}
	return nullptr;
}


QSharedPointer<AddressBook> Node::addressBook()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return  mAddressBook;
	}
	return nullptr;
}


QSharedPointer<PlanBook> Node::planBook()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return  mPlanBook;
	}
	return nullptr;
}


QSharedPointer<ClientList> Node::clientList()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mClients;
	}
	return nullptr;
}


QSharedPointer<Comms> Node::comms()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mComms;
	}
	return nullptr;
}


QSharedPointer<DiscoveryClient> Node::discoveryClient()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mDiscovery;
	}
	return nullptr;
}


QSharedPointer<ZooClient> Node::zooClient()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mZooClient;
	}
	return nullptr;
}


QSharedPointer<SensorInput> Node::sensorInput()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mSensors;
	}
	return nullptr;
}


QSharedPointer<Associate> Node::nodeIdentity()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mNodeIdentity;
	}
	return nullptr;
}


QSharedPointer<ServiceLevelManager> Node::serviceLevelManager()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mServiceLevelManager;
	}
	return nullptr;
}


QUrl Node::serverURL()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		return mServerURL;
	}
	return QUrl();
}


void Node::setPanic(bool panic)
{
	OC_METHODGATE();
	if(mPanic != panic) {
		mPanic = panic;
		//qWarning() << "Panic set to: " << mPanic;
	}
}


bool Node::panic()
{
	OC_METHODGATE();
	return mPanic;
}


void Node::unbirth()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		mNodeIdentity.clear();
		mKeyStore->clear();
		mLocalIdentityStore->clear();
		mAddressBook->clear();
		mPlanBook->clear();
		mClients->clear();
	}
}

void Node::scrape()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		unbirth();
		QFile(mKeyStore->store().filename()).remove();
		QFile(mLocalIdentityStore->filename()).remove();
		QFile(mAddressBook->filename()).remove();
		QFile(mPlanBook->filename()).remove();
	}
}


void Node::applyAppStyle()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		StyleManager styleManager;
		AppStyle style(nodeType());
		styleManager.apply(style);
	}
}


void Node::applyWindowStyle()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		StyleManager styleManager;
		AppStyle style(nodeType());
		auto window = appWindow();
		if(window){
			styleManager.apply(style, *window);
		}
	}
}


bool Node::createBaseDir()
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		const auto dir=context()->baseDir();
		const bool ok=QDir().mkpath(dir);
		if(!ok) {
			qWarning()<<"ERROR: Could not create basedir '"<<dir<<"' for node";
		}
		return ok;
	}
	return false;
}


void Node::discoveryActivate(const bool on)
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		mServiceLevelManager->enableLevel(mDiscoveryServiceLevel->name(), on, [=](bool ok) {
			if(mDebug){
				qDebug() << "discoveryActivate returned: " << (on?"ON":"OFF") << ok;
			}
			if(on){}
		});
	}
}



// Below this line is unrefined
////////////////////////////////////////////////////////////////////////////////



void Node::setNodeIdentity(QVariantMap map)
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		setNodeIdentity(map.isEmpty()?nullptr:QSharedPointer<Associate>::create(map));
	}
}


void Node::setNodeIdentity(QSharedPointer<Associate> nodeID)
{
	OC_METHODGATE();
	if(mAppConfigureHelper.isConfiguredAsExpected()) {
		mNodeIdentity = nodeID;
		if(!mNodeIdentity.isNull()) {
			auto map = mNodeIdentity->toVariantMap();
			if(mDebug){
				qDebug()<<" * * * NEW LOCAL IDENTITY PROVIDED: "<<map<< " FROM nodeID="<<*mNodeIdentity;
			}
			mLocalIdentityStore->fromMap(map);
			/*
			mLocalIdentityStore->synchronize([=](ASEvent<QVariantMap> &ase2) {
				Q_UNUSED(ase2);
			});
			*/
			mLocalIdentityStore->save([this](QSharedPointer<SimpleDataStore>, bool ok) {
				if(mDebug){
					qDebug()<<" HOPEFULLY WE SAVED IDENTITY TO DISK: "<<mNodeIdentity->toString()<<" OK="<<ok;
				}
				if(ok) {
					emit identityChanged();
				}
			});
		}
		else{
			emit identityChanged();
		}
	}
}



QSharedPointer<Node> Node::sharedThis()
{
	OC_METHODGATE();
	qWarning()<<"ERROR: Trying to get shared this from base class Node. Use sharedThisNode() if you really want this.";
	return  QSharedPointer<Node>(nullptr);
}


/*
bool Node::needsConnection()
{
	OC_METHODGATE();
	bool out=false;
	QSharedPointer<Associate> ni=nodeIdentity();
	if(!ni.isNull()) {
		const QString key=NODE_ONLINE_STATUS_BASE_KEY + ni->id();
		QSharedPointer<Settings> s=settings();
		if(!s.isNull()) {
			out=s->getCustomSettingBool(key, out);
		}
	}
	return out;
}
*/

/*
void Node::setNeedsConnection(const bool current)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"NODE::set needs connection: "<<current;
	}
	QSharedPointer<Associate> ni=nodeIdentity();
	QSharedPointer<Settings> s=settings();
	if(!ni.isNull() && !s.isNull()) {
		const QString key=NODE_ONLINE_STATUS_BASE_KEY + ni->id();
		const bool last=s->getCustomSettingBool(key, false);
		if(current!=last) {
			s->setCustomSettingBool(key, current);
			updateCouriersRegistration();
			mComms->updateConnection();
		}
	}
}
*/

/*
bool Node::isConnected()
{
	OC_METHODGATE();
	if(!mComms.isNull()) {
		return mComms->carrier()->isConnected();
	} else {
		qWarning()<<"ERROR: No comms";
	}
	return false;
}


void Node::setConnected(bool val)
{
	OC_METHODGATE();
	//TODO: Implement
	(void)val;
}
*/


// [Un]register node specific couriers with comms
void Node::registerNodeCouriers(const bool reg)
{
	OC_METHODGATE();
	if(!mComms.isNull()) {
		if(nullptr!=mSensorsCourier) {
			mComms->registerCourier(mSensorsCourier, reg);
		}
		if(nullptr!=mBlobCourier) {
			mComms->registerCourier(mBlobCourier, reg);
		}
	}
}


// [Un]register client specific couriers with comms
void Node::registerClientCouriers(const bool reg)
{
	OC_METHODGATE();
	if(!mClients.isNull()) {
		mClients->registerAllCouriers(reg);
	} else {
		qWarning()<<"ERROR: No clientList";
	}
}


// [Un]register both node and client couriers with comms
void Node::registerCouriers(const bool reg)
{
	OC_METHODGATE();
	registerNodeCouriers(reg);
	registerClientCouriers(reg);
}


// Update client specific courier registration with comms depending on need
void Node::updateClientCouriersRegistration()
{
	OC_METHODGATE();
	if(!mClients.isNull()) {
		mClients->updateCourierRegistration();
	} else {
		qWarning()<<"ERROR: No clientList";
	}
}


// Update node specific courier registration with comms depending on need
void Node::updateNodeCouriersRegistration()
{
	OC_METHODGATE();
	// No longer the way to do it as all "needs connection" stuff no resides in carrier:
	//setNodeCouriersRegistration(needsConnection());

}



// Update courier registration with comms depending on need
void Node::updateCouriersRegistration()
{
	OC_METHODGATE();
	updateClientCouriersRegistration();
	updateNodeCouriersRegistration();
}




/*
BlobFuture Node::submitBlobForSending(QByteArray data, QString name)
{
	OC_METHODGATE();
	if(nullptr!=mBlobCourier) {
		BlobFuture future=mBlobCourier->submitSendingBlob(name,data);
		future.connect(*this);
		return future;
	}
	BlobFuture future(name);
	future.fail("No BLobCourier");
	return future;
}
*/

////////////////////////////////



void Node::setHookColorSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if(!mSensorsCourier.isNull()) {
		if(hook) {
			if(!connect(&ob, SIGNAL(colorChanged(QColor)), mSensorsCourier.data(), SLOT(onColorUpdated(QColor)), OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect " << ob.objectName();
			}
		} else {
			if(!disconnect(&ob, SIGNAL(colorChanged(QColor)), mSensorsCourier.data(), SLOT(onColorUpdated(QColor)))) {
				qWarning()<<"ERROR: Could not disconnect " << ob.objectName();
			}
		}
	}
}




////////////////////////////////


void Node::setHookSensorSignals(QObject &o, bool hook)
{
	OC_METHODGATE();
	if(nullptr!=mSensors) {
		mSensors->hookSignals(o, hook);
	}
}


void Node::setHookCommsSignals(QObject &o, bool hook)
{
	OC_METHODGATE();
	if(!mComms.isNull()) {
		// TODO: Figure out if this is useful
		//mComms->hookCommsSignals(o, hook);
	}
}


//////////////////////////////////////////////////
// Key Store slots

/*
void Node::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	//qDebug()<<"Key Store READY="<<mKeystore.isReady()<<", ERROR="<<mKeystore.hasError();
}
*/



//////////////////////////////////////////////////
// CommsChannel slots


void Node::onCommsError(QString e)
{
	Q_UNUSED(e);
	OC_METHODGATE();
	if(mDebug){
		qWarning()<<"NODE UNIMP Comms error: "<<e;
	}
}


void Node::onCommsClientAdded(CommsSession *c)
{
	Q_UNUSED(c);
	OC_METHODGATE();
	if(mDebug){
		qDebug()<<"NODE UNIMP Client added: "<<c->toString();
	}
}


void Node::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)
{
	Q_UNUSED(isConnected);
	Q_UNUSED(needsConnection);
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "NODE UNIMP New connection status: " << (isConnected?"ONLINE":"OFFLINE");
	}
}



//////////////////////////////////////////////////
// Internal sensor slots

void Node::onPositionUpdated(const QGeoPositionInfo &info)
{
	Q_UNUSED(info);
	OC_METHODGATE();
}


void Node::onCompassUpdated(QCompassReading *r)
{
	Q_UNUSED(r);
	OC_METHODGATE();
}


void Node::onAccelerometerUpdated(QAccelerometerReading *r)
{
	Q_UNUSED(r);
	OC_METHODGATE();
}


void Node::onGyroscopeUpdated(QGyroscopeReading *r)
{
	Q_UNUSED(r);
	OC_METHODGATE();
}


/*
CameraList *Node::cameraList()
{
	OC_METHODGATE();
	return mCameras;
}
*/


/*
QSet<QSharedPointer<Associate> > Node::allControls()
{
	OC_METHODGATE();
	QSet<QSharedPointer<Associate> > out;
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		AddressBook &p=addressBook();
		out = p.all().values().toSet();
	}
	return out;
}
*/

//withActiveSessions(CommsSessionDirectory &sessionDirectory, AddressBook &ab, quint64 now, const bool honeyMoon)


/*
QSet<QSharedPointer<Associate> > Node::controlsWithActiveSessions(quint64 now)
{
	OC_METHODGATE();
	QSet<QSharedPointer<Associate> > out;
	if(0==now) {
		now=utility::time::currentMsecsSinceEpoch<quint64>();
	}
	const quint64 lastActive=now-(1000*60);//One minute ago. TODO: Turn into constant or setting

	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		AddressBook &p=addressBook();
		CommsSessionDirectory &sessionDirectory=cc->sessions();
		const bool honeyMoon=cc->honeymoon(now);
		QSet<QSharedPointer<CommsSession> > sessions = ( (honeyMoon)? sessionDirectory.all() : sessionDirectory.byActiveTime(lastActive));
		//qDebug()<<"HoneyMoon enabled="<<honeyMoon<< " gave "<<sessions.size()<<" sessions";
		for(QSharedPointer<CommsSession> session: sessions) {
			auto key=session->key();
			if(nullptr!=key) {
				QString id=key->id();
				if(id.size()>0) {
					QSharedPointer<Associate> peer=p.associateByID(id);
					if(nullptr!=peer) {
						if(NodeRole::ROLE_CONTROL==peer->role()) {
							out<<peer;
						}
					}
				}
			} else {
				qWarning()<<"ERROR: No key";
			}
		}
	} else {
		qWarning()<<"ERROR: No comms";
	}

	return out;
}
*/


/*
QSharedPointer<Node> Node::sharedThisNode()
{
	OC_METHODGATE();
	return QEnableSharedFromThis<Node>::sharedFromThis();
}
*/



//////////////////////////////////////////////////////////////
////////////// NOT ENABLED ///////////////////////////////////
//////////////////////////////////////////////////////////////
/*
Shower ideas:
1. ClientFactory DONE
	2. Create ClientStatus DONE (future to decouple Client from all sorts of stuff)
	3. client needs  DONE (needsConnection(), isConnected(), setConnected(bool) and setNeedsConnection())
*/

//////////////////////////////////////////////////////////////
////////////// NOT ENABLED ///////////////////////////////////
//////////////////////////////////////////////////////////////

/*
bool Node::courierRegistration()
{
	OC_METHODGATE();
	return false;
}
*/
//////////////////////////////////////////////////////////////
////////////// NOT ENABLED ///////////////////////////////////
//////////////////////////////////////////////////////////////

/*
void Node::setCourierRegistration(QSharedPointer<Associate> assoc, bool reg)
{
	OC_METHODGATE();
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		if(!assoc.isNull()) {
			//quint64 now=0;
			//if(0==now) {				now=utility::time::currentMsecsSinceEpoch<quint64>();			}
			//const bool honeyMoon=cc->honeymoon(now);
			//QSet<QSharedPointer<NodeAssociate> > controls = honeyMoon? allControls() : controlsWithActiveSessions(now);
			const QString id=assoc->id();
			if(reg) {
				qDebug()<<"REGISTERING COURIERS FOR " <<id;
				QSet< QSharedPointer<Courier> > set;
				Courier *courier=OC_NEW SensorsCourier(*cc);
				set<< QSharedPointer<Courier>(courier);
				//QMap<const QString, QSet< QSharedPointer<Courier> > > mCourierSets;
				mCourierSets.insert(id, set);
				cc->setCourierRegistered(*courier, true);

			} else {
				qDebug()<<"UN-REGISTERING COURIERS FOR " <<id;
				if(mCourierSets.contains(id)) {
					QSet< QSharedPointer<Courier> > set=mCourierSets.take(id);
					for(QSharedPointer<Courier>  courier:set) {
						if(nullptr!=courier) {

							cc->setCourierRegistered(*courier, false);
						}
					}
				}
			}
		}
	}
}
*/

//////////////////////////////////////////////////////////////
////////////// NOT ENABLED ///////////////////////////////////
//////////////////////////////////////////////////////////////

/*
void Node::updateCourierRegistration(quint64 now)
{
	OC_METHODGATE();
	if(0==now) {
		now=utility::time::currentMsecsSinceEpoch<quint64>();
	}
	QSet<QSharedPointer<Associate> > active = controlsWithActiveSessions(now);
	if(active!=mLastActiveControls) {
		for(QSharedPointer<Associate> assoc:mLastActiveControls) {
			if(!active.contains(assoc)) {
				//Decomission it
				setCourierRegistration(assoc,false);
			}
		}
		for(QSharedPointer<Associate> assoc:active) {
			if(!mLastActiveControls.contains(assoc)) {
				//Comission it
				setCourierRegistration(assoc,true);
			}
		}
	}
	mLastActiveControls.clear();
	mLastActiveControls = active;
}

//////////////////////////////////////////////////////////////
////////////// NOT ENABLED ///////////////////////////////////
//////////////////////////////////////////////////////////////


// NOTE: This has a sister method in Client.cpp around line 73
//       Please see the comments there as they are also relevant here
//       (about for example in what really important way these two methods differ).

TryToggleState Node::updateOnlineStatus(const TryToggleState currentTryState)
{
	OC_METHODGATE();
	// Find if we are TRYING to go online
	const bool isTryingToGoOnline=isConnected();
	// Find if we WANT to be online
	const bool wantToBeOnline=isTryingToGoOnline=needsConnection();
	Settings *s=&settings();
	if(nullptr!=s) {
		wantToBeOnline=s->getCustomSettingBool(AgentConstants::AGENT_CONNECTION_STATUS, wantToBeOnline);
	} else {
		qWarning()<<"ERROR: No settings for agent";
	}
	//Spell it out for debugging
	QSharedPointer<Associate> associate=nodeIdentity();
	if(!associate.isNull()) {
		const QString localID=associate->id();
		qDebug()<<"Agent '"<<localID<<"' is currently trying to be "<<(isTryingToGoOnline?"ONLINE":"OFFLINE")<<" and we now want to try for "<<(wantToBeOnline?"ONLINE":"OFFLINE")<<".";
		// Make necessary changes to state
		TryToggleState nextTryState=currentTryState;
		bool nextOnlineStatus=isTryingToGoOnline;
		if(wantToBeOnline ) {
			if(isTryingToGoOnline ) {
				nextTryState=ON;
			} else {
				nextTryState=GOING_ON;
				qDebug()<<"Decided to go online";
				nextOnlineStatus=true;
			}
		} else {
			if(isTryingToGoOnline ) {
				qDebug()<<"Decided to go offline";
				nextOnlineStatus=false;
			} else {
				nextTryState=OFF;
			}
		}
		if(nextOnlineStatus!=isTryingToGoOnline) {
			comms()->setHoneymoonEnd(utility::time::currentMsecsSinceEpoch<quint64>()+(1000*60*5));//Set 5 minute honeymoon at every state change
			QSet<QSharedPointer<Associate> > all=allControls();
			qDebug()<<"Decided to change online status for "<<QString::number(all.size())<< " controls:";
			for(auto assoc:all) {
				const QString assocID= assoc->id();
				if(assocID == localID) {
					qDebug()<<" + Skipping ME";
					continue;
				}
				qDebug()<<" + " << assocID;
				setCourierRegistration(assoc , nextOnlineStatus); //QSharedPointer<NodeAssociate> assoc, bool reg
			}

		} else {
			//qDebug()<<"No change in online status ("<<nextOnlineStatus<<")";
		}
		if(nextTryState!=currentTryState) {
			//qDebug()<<"Decided to change tristate button from "<<current<<" -> "<<next;
			return nextTryState;
		} else {
			//qDebug()<<"No change tristate button ("<<current<<")";
		}
	}
	return currentTryState;

}
*/
