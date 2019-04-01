/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "Node.hpp"


#include "camera/CameraList.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"


#include "address/Associate.hpp"
#include "discovery/AddressBook.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "hardware/sensors/SensorInput.hpp"
#include "client/zoo/ZooClient.hpp"


#include "uptime/ConnectionType.hpp"
#include "agent/AgentConstants.hpp"
#include "app/AppContext.hpp"
#include "app/Constants.hpp"
#include "app/launcher/IAppLauncher.hpp"
#include "app/style/StyleManager.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/time/ScopedTimer.hpp"


#include <QCommandLineParser>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QDir>


static const QString NODE_ONLINE_STATUS_BASE_KEY("octomy.node.online.");

quint16 defaultPortForNodeType(NodeType type)
{
	quint16 defaultPort=0;
	switch(type) {
	case(TYPE_ZOO):
	//default:
	case(TYPE_UNKNOWN):
	case(TYPE_AGENT): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_AGENT;
	}
	break;
	case(TYPE_REMOTE): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_REMOTE;
	}
	break;
	case(TYPE_HUB): {
		defaultPort=Constants::OCTOMY_UDP_DEFAULT_PORT_HUB;
	}
	break;
	}
	return defaultPort;
}


Node::Node()
	: QObject(nullptr)
	, mAppConfigureHelper("node", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	, mAddresses(OC_NEW LocalAddressList())
	, mLocalIdentity(OC_NEW LocalIdentityStore())
	, mAddressBook(OC_NEW AddressBook())
	, mLastStatusSend (0)
	, mServerURL("http://zoo.octomy.org:"+QString::number(Constants::OCTOMY_UDP_DEFAULT_PORT_ZOO)+"/api") //pointed to localhost using /etc/hosts
{
	OC_METHODGATE();
	//qDebug()<<"Node()";
}


Node::~Node()
{
	OC_METHODGATE();
	//qDebug()<<"~Node()";
}

// TODO: Look at simplifying init system by combining appInit and appConfigure
//  , renaming appDeInit to appDeConfigure, and maybe removing one layer so there
//	is not appXXX + nodeXXX but just one of them.

void Node::appConfigure(QSharedPointer<IAppLauncher> launcher)
{
	OC_METHODGATE();
	if(mAppConfigureHelper.configure()) {
		//qDebug()<<"appConfigure()";
		mLauncher=launcher;
		auto ctx=context();
		if(!ctx.isNull()) {
			//	, mKeyStore (context()->baseDir() + "/keystore.json", true)


			//mAddresses->configure();
			//mLocalIdentity->configure(ctx->baseDir() + "/local_identity.json");
			//mAddressBook->configure(ctx->baseDir() + "/addressbook.json");

			//	, mAddresses(defaultPortForNodeType(mType), true)
			//	, mCarrier(OC_NEW CommsCarrierUDP( static_cast<QObject *>(this)) )
			//	, mComms (OC_NEW CommsChannel(*mCarrier, mKeyStore, mAddressBook, static_cast<QObject *>(this)))
			//	, mDiscovery (OC_NEW DiscoveryClient(*this))
			//	, mZooClient (OC_NEW ZooClient(this))
			//	, mSensors (OC_NEW SensorInput(this))
			//	, mSensorsCourier(QSharedPointer<SensorsCourier>(OC_NEW SensorsCourier(*mComms, this)))
			//	, mBlobCourier(QSharedPointer<BlobCourier>(OC_NEW BlobCourier(*mComms, this)))
			//	, mCameras (OC_NEW CameraList(this))

			// ScopedTimer nodeBootTimer(context()->base()+"-boot");
			setObjectName(ctx->base());
			nodeConfigure();
		} else {
			qWarning()<<"ERROR: No context";
		}
	}
}


void Node::appActivate(const bool on)
{
	OC_METHODGATE();
	//qDebug()<<"appActivate()";

	if(mAppConfigureHelper.activate(on)) {
		if(on) {
			/*
			if(nullptr!= mCarrier) {
				const NetworkAddress listenAddress(QHostAddress::Any, mAddresses.port());
				mCarrier->setListenAddress(listenAddress);
			}
			*/

			/*
			mKeyStore.synchronize([this](ASEvent<QVariantMap> &se) {
				const bool ok=se.isSuccessfull();
				qDebug()<<"------------------------------------------------";
				qDebug()<<"Keystore synchronized with ok="<<ok;
				mKeyStore.dump();
				qDebug()<<"------------------------------------------------";
			});
			*/

			/*
			mLocalIdentity.setInitialized(&mLocalIdentity);
			mLocalIdentity.synchronize([this](SimpleDataStore &sms, bool ok) {
				auto map=sms.toMap();
				//qDebug()<<"Local identity synchronized with ok="<<ok<<" and map="<<map;
				if(ok && !map.isEmpty()) {
					identityChanged();
				}
			});
			*/

			/*
				mAddressBook->setInitialized(mAddressBook.data());
				mAddressBook->synchronize([this](SimpleDataStore &ab, bool ok) {
					Q_UNUSED(ab);
					Q_UNUSED(ok);
					//qDebug()<<"Address book synchronized with ok="<<ok;
					mClients.syncToAddressBook(mAddressBook, sharedThis());
				});
				*/


			/*
			StyleManager *style=OC_NEW StyleManager(QColor(TYPE_AGENT==mType?"#e83636":TYPE_REMOTE==mType?"#36bee8":"#36e843"));
			if(nullptr!=style) {
				style->apply();
			}
			*/

			/*
			if(!QDir().mkpath(context()->baseDir())) {
				qWarning()<<"ERROR: Could not create basedir for node";
			}
			*/


			//setHookSensorSignals(*this, true);
			//setHookSensorSignals(*mSensorsCourier, true);

			//setHookCommsSignals(*this,true);

			/*
			if(nullptr!=mZooClient) {
				mZooClient->setURL(mServerURL);
			}
			*/

			/*
			if(nullptr!=mDiscovery) {
				mDiscovery->setURL(mServerURL);
			}
			*/

			// Bootstrap connection
			/*
			const bool last=needsConnection();
			setNeedsConnection(false);
			setNeedsConnection(last);
			*/
		} else {
			setHookSensorSignals(*this, false);
			setHookCommsSignals(*this, false);

			//mAddressBook.setInitialized<AddressBook>(nullptr);
			//mLocalIdentity.setInitialized<LocalIdentityStore>(nullptr);
			/*
			if(nullptr!=mCarrier) {
				mCarrier->deleteLater();
				mCarrier=nullptr;
			}
			*/
			/*
			if(nullptr!=mComms) {
				mComms->deleteLater();
				mComms=nullptr;
			}
			*/
			/*
			if(nullptr!=mDiscovery) {
				mDiscovery->deleteLater();
				mDiscovery=nullptr;
			}
			*/
			/*
			if(nullptr!=mZooClient) {
				mZooClient->deleteLater();
				mZooClient=nullptr;
			}
			*/
			/*
			if(nullptr!=mSensors) {
				mSensors->deleteLater();
				mSensors=nullptr;
			}
			*/
			/*
			if(nullptr!=mSensorsCourier) {
				mSensorsCourier->deleteLater();
				mSensorsCourier=nullptr;
			}
			*/
			/*
			if(nullptr!=mBlobCourier) {
				mBlobCourier->deleteLater();
				mBlobCourier=nullptr;
			}
			*/
			/*
			if(nullptr!=mCameras) {
				mCameras->deleteLater();
				mCameras=nullptr;
			}
			*/

		}
		// NOTE: It is expected that this emits nodeActivateChanged() at some point
		nodeActivate(on);
	}
}





QSharedPointer<QWidget> Node::appWindow()
{
	OC_METHODGATE();
	//qDebug()<<"appWindow()";
	return nodeWindow();
}


QSharedPointer<IAppLauncher> Node::launcher()
{
	OC_METHODGATE();
	return mLauncher;
}



QSharedPointer<AppContext> Node::context()
{
	OC_METHODGATE();
	return mLauncher.isNull()?nullptr:mLauncher->context();
}


QSharedPointer<Settings> Node::settings()
{
	OC_METHODGATE();
	auto c=this->context();
	return c.isNull()?nullptr:c->settings();
}

QSharedPointer<LocalIdentityStore> Node::localIdentityStore()
{
	OC_METHODGATE();
	return mLocalIdentity;
}


QSharedPointer<KeyStore> Node::keyStore()
{
	OC_METHODGATE();
	return mKeyStore;
}


QSharedPointer<AddressBook> Node::addressBook()
{
	OC_METHODGATE();
	return  mAddressBook;
}


/*
QSharedPointer<ClientList> Node::clientList()
{
	OC_METHODGATE();
	return mClients;
}
*/

QSharedPointer<LocalAddressList> Node::localAddressList()
{
	OC_METHODGATE();
	return mAddresses;
}


QSharedPointer<CommsChannel> Node::comms()
{
	OC_METHODGATE();
	return mComms;
}


QSharedPointer<DiscoveryClient> Node::discoveryClient()
{
	OC_METHODGATE();
	return mDiscovery;
}


QSharedPointer<ZooClient> Node::zooClient()
{
	OC_METHODGATE();
	return mZooClient;
}


QSharedPointer<SensorInput> Node::sensorInput()
{
	OC_METHODGATE();
	return mSensors;
}


QSharedPointer<Associate> Node::nodeIdentity()
{
	OC_METHODGATE();
	return mNodeIdentity;
}


// Below this line is unrefined
////////////////////////////////////////////////////////////////////////////////


void Node::unbirth()
{
	OC_METHODGATE();
	//mKeyStore.clear();
	//mLocalIdentity.clear();
	//mAddressBook.clear();
	//mClients.clear();
}



QString Node::name()
{
	OC_METHODGATE();
	/*
	QSharedPointer<Associate>  me=nodeIdentity();
	QString name;
	if(nullptr!=me) {
		name=me->name().trimmed();
	}
	if(""!=name) {
		switch(mType) {
		case(TYPE_AGENT): {
			name="Agent ";
		}
		break;
		case(TYPE_HUB): {
			name="Hub ";
		}
		break;
		case(TYPE_REMOTE): {
			name="Remote ";
		}
		break;
		case(TYPE_ZOO): {
			name="Zoo ";
		}
		break;
		default:
			name="Unknown ";
		};
		name+=me->id().mid(0,8);
	}
	*/
	return "";
}



void Node::setNodeIdentity(QVariantMap map)
{
	OC_METHODGATE();
	mNodeIdentity=QSharedPointer<Associate>(OC_NEW Associate(map, true));
	//qDebug()<<" * * * NEW LOCAL IDENTITY PROVIDED BY MAP: "<<map;
	mLocalIdentity->fromMap(map);
	mLocalIdentity->save();
	identityChanged();
}

void Node::setNodeIdentity(QSharedPointer<Associate> nodeID)
{
	OC_METHODGATE();
	mNodeIdentity=nodeID;
	QVariantMap map;
	if(!mNodeIdentity.isNull()) {
		map=mNodeIdentity->toVariantMap();
	}
	//qDebug()<<" * * * NEW LOCAL IDENTITY PROVIDED: "<<map<< " FROM nodeID="<<*mNodeIdentity;
	mLocalIdentity->fromMap(map);
	mLocalIdentity->save();
	identityChanged();
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

void Node::identityChanged()
{
	OC_METHODGATE();
}

QSharedPointer<Node> Node::sharedThis()
{
	OC_METHODGATE();
	qWarning()<<"ERROR: Trying to get shared this from base class Node";
	return  QSharedPointer<Node>(nullptr);
}


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


void Node::setNeedsConnection(const bool current)
{
	OC_METHODGATE();
	//qDebug()<<"NODE::set needs connection: "<<current;
	QSharedPointer<Associate> ni=nodeIdentity();
	QSharedPointer<Settings> s=settings();
	if(!ni.isNull() && !s.isNull()) {
		const QString key=NODE_ONLINE_STATUS_BASE_KEY + ni->id();
		const bool last=s->getCustomSettingBool(key, false);
		if(current!=last) {
			s->setCustomSettingBool(key, current);
			updateCouriersRegistration();
			mComms->updateConnect();
		}
	}
}


bool Node::isConnected()
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		return mComms->carrier().isConnected();
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



// [Un]register node specific couriers with comms

void Node::setNodeCouriersRegistration(const bool reg)
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		if(nullptr!=mSensorsCourier) {
			mComms->setCourierRegistered(mSensorsCourier, reg);
		}
		if(nullptr!=mBlobCourier) {
			mComms->setCourierRegistered(mBlobCourier, reg);
		}
	}
}



// [Un]register client specific couriers with comms
void Node::setClientCouriersRegistration(const bool reg)
{
	OC_METHODGATE();
	mClients.setAllCouriersRegistered(reg);
}



/*
// [Un]register couriers with comms
void Node::setCouriersRegistration(const bool reg)
{
	OC_METHODGATE();
	setNodeCouriersRegistration(reg);
	setClientCouriersRegistration(reg);
}
*/



// Update client specific courier registration with comms depending on need
void Node::updateClientCouriersRegistration()
{
	OC_METHODGATE();
	mClients.updateCourierRegistration();
}




// Update node specific courier registration with comms depending on need
void Node::updateNodeCouriersRegistration()
{
	OC_METHODGATE();
	setNodeCouriersRegistration(needsConnection());
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
	if(nullptr!=mComms) {
		mComms->setHookCommsSignals(o, hook);
	}
}




void Node::updateDiscoveryClient()
{
	OC_METHODGATE();
	mDiscovery=QSharedPointer<DiscoveryClient>(OC_NEW DiscoveryClient());
	mDiscovery->configure(sharedThis());
	mDiscovery->setURL(mServerURL);
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
	//qDebug()<<"NODE UNIMP Comms error: "<<e;
}

void Node::onCommsClientAdded(CommsSession *c)
{
	Q_UNUSED(c);
	OC_METHODGATE();
	//qDebug()<<"NODE UNIMP Client added: "<<c->toString();
}

void Node::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)
{
	Q_UNUSED(isConnected);
	Q_UNUSED(needsConnection);
	OC_METHODGATE();
	//qDebug() <<"NODE UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
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

