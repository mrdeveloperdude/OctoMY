/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "Node.hpp"


#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "zoo/ZooClient.hpp"
#include "camera/CameraList.hpp"
#include "sensory/SensorInput.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"
#include "utility/Utility.hpp"
#include "utility/ScopedTimer.hpp"
#include "basic/StyleManager.hpp"
#include "AppContext.hpp"
#include "zoo/ZooConstants.hpp"
#include "agent/AgentConstants.hpp"


#include <QCommandLineParser>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QDir>


static const QString NODE_ONLINE_STATUS_BASE_KEY("octomy.node.online.");

static quint16 defaultPortForNodeType(NodeType type)
{
	quint16 defaultPort=0;
	switch(type) {
	case(TYPE_ZOO):
	default:
	case(TYPE_UNKNOWN):
	case(TYPE_AGENT): {
		defaultPort=ZooConstants::OCTOMY_UDP_DEFAULT_PORT_AGENT;
	}
	break;
	case(TYPE_REMOTE): {
		defaultPort=ZooConstants::OCTOMY_UDP_DEFAULT_PORT_REMOTE;
	}
	break;
	case(TYPE_HUB): {
		defaultPort=ZooConstants::OCTOMY_UDP_DEFAULT_PORT_HUB;
	}
	break;
	}
	return defaultPort;
}


Node::Node(AppContext *context, NodeRole role, NodeType type, QObject *parent)
	: QObject (parent)
	, mContext(context)
	, mRole (role)
	, mType (type)
	, mKeystore (mContext->baseDir() + "/keystore.json")
	, mAddressBook (mContext->baseDir() + "/addressbook.json")
	, mAddresses(defaultPortForNodeType(mType), true)
	, mCarrier(OC_NEW CommsCarrierUDP( (QObject *)this) )
	, mComms (OC_NEW CommsChannel(*mCarrier, mKeystore, mAddressBook, (QObject *)this))
	, mDiscovery (OC_NEW DiscoveryClient(*this))
	, mZooClient (OC_NEW ZooClient(this))
	, mSensors (OC_NEW SensorInput(this))
	, mSensorsCourier(QSharedPointer<SensorsCourier>(OC_NEW SensorsCourier(*mComms, this)))
	, mBlobCourier(QSharedPointer<BlobCourier>(OC_NEW BlobCourier(*mComms, this)))
	, mCameras (OC_NEW CameraList(this))
	, mLastStatusSend (0)
	, mServerURL("http://zoo.octomy.org:"+QString::number(ZooConstants::OCTOMY_UDP_DEFAULT_PORT_ZOO)+"/api") //pointed to localhost using /etc/hosts
{
	OC_METHODGATE();
	//ScopedTimer nodeBootTimer(mContext->base()+"-boot");
	setObjectName(mContext->base());

	init();
}

Node::~Node()
{
	OC_METHODGATE();
	deInit();
}



void Node::init()
{
	OC_METHODGATE();
	mKeystore.setHookSignals(*this, true);
	// Only Agents are "born"
	mKeystore.bootstrap(ROLE_AGENT==mRole);

	mAddressBook.bootstrap(true, false);

	StyleManager *style=OC_NEW StyleManager(QColor(TYPE_AGENT==mType?"#e83636":TYPE_REMOTE==mType?"#36bee8":"#36e843"));
	if(nullptr!=style) {
		style->apply();
	}

	if(!QDir().mkpath(mContext->baseDir())) {
		qWarning()<<"ERROR: Could not create basedir for node";
	}

	setHookSensorSignals(*this, true);
	setHookSensorSignals(*mSensorsCourier, true);

	setHookCommsSignals(*this,true);

	if(nullptr!=mZooClient) {
		mZooClient->setURL(mServerURL);
	}

	if(nullptr!=mDiscovery) {
		mDiscovery->setURL(mServerURL);
	}

	// Bootstrap connection
	const bool last=needsConnection();
	setNeedsConnection(false);
	setNeedsConnection(last);
}


void Node::deInit()
{
	setHookSensorSignals(*this, false);
	setHookCommsSignals(*this,false);
	mKeystore.setHookSignals(*this, false);
}




NodeRole Node::role()
{
	OC_METHODGATE();
	return mRole;
}


NodeType Node::type()
{
	OC_METHODGATE();
	return mType;
}


KeyStore  &Node::keyStore()
{
	OC_METHODGATE();
	return mKeystore;
}

AddressBook &Node::addressBook()
{
	OC_METHODGATE();
	return  mAddressBook;
}


LocalAddressList &Node::localAddressList()
{
	OC_METHODGATE();
	return mAddresses;
}

ClientList &Node::clientList()
{
	OC_METHODGATE();
	return mClients;
}


QString Node::name()
{
	OC_METHODGATE();
	QSharedPointer<Associate>  me=nodeIdentity();
	QString name;
	if(nullptr!=me) {
		name=me->name();
	}
	if(""!=name.trimmed()) {
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
	return "";
}

CommsChannel *Node::comms()
{
	OC_METHODGATE();
	return mComms;
}


DiscoveryClient *Node::discoveryClient()
{
	OC_METHODGATE();
	return mDiscovery;
}

ZooClient *Node::zooClient()
{
	OC_METHODGATE();
	return mZooClient;
}

SensorInput *Node::sensorInput()
{
	OC_METHODGATE();
	return mSensors;
}

//TODO: Should we really store our own data in the address book... I mean... yeahh..
QSharedPointer<Associate> Node::nodeIdentity()
{
	OC_METHODGATE();
	auto key=mKeystore.localKey();
	QSharedPointer<Associate> me=(!key.isNull())?mAddressBook.associateByID(key->id()):nullptr;
	return me;
}

CameraList *Node::cameraList()
{
	OC_METHODGATE();
	return mCameras;
}

const QCommandLineParser &Node::commandLine() const
{
	OC_METHODGATE();
	return mContext->options();
}

Settings &Node::settings()
{
	OC_METHODGATE();
	return mContext->settings();
}



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

//withActiveSessions(CommsSessionDirectory &sessionDirectory, AddressBook &ab, quint64 now, const bool honeyMoon)

QSet<QSharedPointer<Associate> > Node::controlsWithActiveSessions(quint64 now)
{
	OC_METHODGATE();
	QSet<QSharedPointer<Associate> > out;
	if(0==now) {
		now=QDateTime::currentMSecsSinceEpoch();
	}
	const quint64 lastActive=now-(1000*60);//One minute ago. TODO: Turn into constant or setting

	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		AddressBook &p=addressBook();
		CommsSessionDirectory &sessionDirectory=cc->sessions();
		const bool honeyMoon=cc->honeymoon(now);
		QSet<QSharedPointer<CommsSession> > sessions = ( (honeyMoon)? sessionDirectory.all() : sessionDirectory.byActiveTime(lastActive));
		qDebug()<<"HoneyMoon enabled="<<honeyMoon<< " gave "<<sessions.size()<<" sessions";
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

/*
Shower ideas:
1. ClientFactory DONE
	2. Create ClientStatus DONE (future to decouple Client from all sorts of stuff)
	3. client needs  DONE (needsConnection(), isConnected(), setConnected(bool) and setNeedsConnection())
*/

/*
bool Node::courierRegistration()
{
	OC_METHODGATE();
	return false;
}
*/
/*
void Node::setCourierRegistration(QSharedPointer<Associate> assoc, bool reg)
{
	OC_METHODGATE();
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		if(!assoc.isNull()) {
			//quint64 now=0;
			//if(0==now) {				now=QDateTime::currentMSecsSinceEpoch();			}
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

/*
void Node::updateCourierRegistration(quint64 now)
{
	OC_METHODGATE();
	if(0==now) {
		now=QDateTime::currentMSecsSinceEpoch();
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
			comms()->setHoneymoonEnd(QDateTime::currentMSecsSinceEpoch()+(1000*60*5));//Set 5 minute honeymoon at every state change
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

QWidget *Node::showWindow()
{
	OC_METHODGATE();
	return nullptr;
}




bool Node::needsConnection()
{
	OC_METHODGATE();
	bool out=false;
	QSharedPointer<Associate> ni=nodeIdentity();
	if(!ni.isNull()) {
		const QString key=NODE_ONLINE_STATUS_BASE_KEY + ni->id();
		out=settings().getCustomSettingBool(key, out);
	}
	return out;
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


// [Un]register node specific couriers with comms
void Node::setNodeCouriersRegistered(const bool reg){
	OC_METHODGATE();
	if(nullptr!=mComms){
		if(nullptr!=mSensorsCourier){
			mComms->setCourierRegistered(mSensorsCourier, reg);
		}
		if(nullptr!=mBlobCourier){
			mComms->setCourierRegistered(mBlobCourier, reg);
		}
	}
}

// [Un]register client specific couriers with comms
void Node::setClientCouriersRegistered(const bool reg){
	OC_METHODGATE();
	mClients.setAllCouriersRegistered(reg);
}


// [Un]register couriers with comms
void Node::setCouriersRegistered(const bool reg){
	OC_METHODGATE();
	setNodeCouriersRegistered(reg);
	setClientCouriersRegistered(reg);
}


// Update client specific courier registration with comms depending on need
void Node::updateClientCouriersRegistered(){
	OC_METHODGATE();
	mClients.updateCourierRegistration();
}


// Update node specific courier registration with comms depending on need
void Node::updateNodeCouriersRegistered(){
	OC_METHODGATE();
	setNodeCouriersRegistered(needsConnection());
}


// Update courier registration with comms depending on need
void Node::updateCouriersRegistered(){
	OC_METHODGATE();
	updateClientCouriersRegistered();
	updateNodeCouriersRegistered();
}


void Node::setNeedsConnection(const bool current)
{
	OC_METHODGATE();
	QSharedPointer<Associate> ni=nodeIdentity();
	if(!ni.isNull()) {
		const QString key=NODE_ONLINE_STATUS_BASE_KEY + ni->id();
		const bool last=settings().getCustomSettingBool(key, false);
		if(current!=last) {
			settings().setCustomSettingBool(key, current);
			updateCouriersRegistered();
			mComms->updateConnect();
		}
	}
}


void Node::setConnected(bool val)
{
	OC_METHODGATE();
	//TODO: Implement
	(void)val;
}


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

////////////////////////////////


void Node::setHookColorSignals(QObject &ob, bool set)
{
	OC_METHODGATE();
	if(set) {
		if(!mSensorsCourier.isNull()) {
			if(!connect(&ob, SIGNAL(colorChanged(QColor)), mSensorsCourier.data(), SLOT(onColorUpdated(QColor)), OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect " << ob.objectName();
			}
		}
	} else {
		if(!mSensorsCourier.isNull()) {
			if(!disconnect(&ob, SIGNAL(colorChanged(QColor)), mSensorsCourier.data(), SLOT(onColorUpdated(QColor)))) {
				qWarning()<<"ERROR: Could not disconnect " << ob.objectName();
			}
		}
	}
}




////////////////////////////////


void Node::setHookSensorSignals(QObject &o, bool set)
{
	OC_METHODGATE();
	if(set) {
		if(nullptr!=mSensors) {
			mSensors->hookSignals(o);
		}
	} else {
		if(nullptr!=mSensors) {
			mSensors->unHookSignals(o);
		}
	}
}


void Node::setHookCommsSignals(QObject &o, bool hook)
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		mComms->setHookCommsSignals(o,hook);
	}
}



void Node::setHookPeerSignals(QObject &o, bool set)
{
	OC_METHODGATE();
	if(set) {
		mAddressBook.hookSignals(o);
	} else {
		mAddressBook.unHookSignals(o);
	}
}



void Node::updateDiscoveryClient()
{
	OC_METHODGATE();
	delete mDiscovery;
	mDiscovery=OC_NEW DiscoveryClient(*this);
	mDiscovery->setURL(mServerURL);
}


//////////////////////////////////////////////////
// Key Store slots

void Node::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	//qDebug()<<"Key Store READY="<<mKeystore.isReady()<<", ERROR="<<mKeystore.hasError();
}


//////////////////////////////////////////////////
// CommsChannel slots


void Node::onCommsError(QString e)
{
	OC_METHODGATE();
	//qDebug()<<"NODE UNIMP Comms error: "<<e;
}

void Node::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	//qDebug()<<"NODE UNIMP Client added: "<<c->toString();
}

void Node::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)
{
	OC_METHODGATE();
	//qDebug() <<"NODE UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}



//////////////////////////////////////////////////
// Internal sensor slots

void Node::onPositionUpdated(const QGeoPositionInfo &info)
{
	OC_METHODGATE();
}


void Node::onCompassUpdated(QCompassReading *r)
{
	OC_METHODGATE();
}

void Node::onAccelerometerUpdated(QAccelerometerReading *r)
{
	OC_METHODGATE();
}

void Node::onGyroscopeUpdated(QGyroscopeReading *r)
{
	OC_METHODGATE();
}
