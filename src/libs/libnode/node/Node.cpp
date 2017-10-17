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


#include <QCommandLineParser>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QDir>


static quint16 defaultPortForNodeType(DiscoveryType type)
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


Node::Node(AppContext *context, DiscoveryRole role, DiscoveryType type, QObject *parent)
	: QObject (parent)
	, mContext(context)
	, mKeystore (mContext->baseDir() + "/keystore.json")
	, mAssociates (mContext->baseDir() + "/addressbook.json")
	, mDiscovery (OC_NEW DiscoveryClient(*this))
	, mRole (role)
	, mType (type)
	, mCarrier(OC_NEW CommsCarrierUDP( (QObject *)this) )
	, mComms (OC_NEW CommsChannel(*mCarrier, mKeystore, mAssociates, (QObject *)this))
	, mZooClient (OC_NEW ZooClient(this))
	, mSensors (OC_NEW SensorInput(this))
	, mSensorsCourier(OC_NEW SensorsCourier(*mComms, this))
	, mBlobCourier(OC_NEW BlobCourier(*mComms, this))
	, mCameras (OC_NEW CameraList(this))
	, mLastStatusSend (0)
	, mServerURL("http://zoo.octomy.org:"+QString::number(ZooConstants::OCTOMY_UDP_DEFAULT_PORT_ZOO)+"/api") //pointed to localhost using /etc/hosts
	, mAddresses(defaultPortForNodeType(mType))
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

	mAssociates.bootstrap(true, false);

	StyleManager *style=OC_NEW StyleManager(QColor(TYPE_AGENT==mType?"#e83636":TYPE_REMOTE==mType?"#36bee8":"#36e843"));
	if(nullptr!=style) {
		style->apply();
	}

	if(!QDir().mkpath(mContext->baseDir())) {
		qWarning()<<"ERROR: Could not create basedir for node";
	}

	hookSensorSignals(*this);
	hookSensorSignals(*mSensorsCourier);

	setHookCommsSignals(*this,true);

	if(nullptr!=mZooClient) {
		mZooClient->setURL(mServerURL);
	}

	if(nullptr!=mDiscovery) {
		mDiscovery->setURL(mServerURL);
	}

}


void Node::deInit()
{
	unHookSensorSignals(*this);
	setHookCommsSignals(*this,false);
	mKeystore.setHookSignals(*this, false);
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

KeyStore  &Node::keyStore()
{
	OC_METHODGATE();
	return mKeystore;
}

AddressBook &Node::peers()
{
	OC_METHODGATE();
	return  mAssociates;
}

DiscoveryClient *Node::discoveryClient()
{
	OC_METHODGATE();
	return mDiscovery;
}

DiscoveryRole Node::role()
{
	OC_METHODGATE();
	return mRole;
}


DiscoveryType Node::type()
{
	OC_METHODGATE();
	return mType;
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

QSharedPointer<Associate> Node::nodeIdentity()
{
	OC_METHODGATE();
	auto key=mKeystore.localKey();
	QSharedPointer<Associate> me=(!key.isNull())?mAssociates.associateByID(key->id()):nullptr;
	return me;
}

CameraList *Node::cameras()
{
	OC_METHODGATE();
	return mCameras;
}

QWidget *Node::showWindow()
{
	OC_METHODGATE();
	return nullptr;
}


NetworkAddress Node::localAddress()
{
	OC_METHODGATE();
	QSharedPointer<Associate> nid=nodeIdentity();
	if(!nid.isNull()) {
		return nid->addressList().bestAddress();
	}
	return NetworkAddress();
}



LocalAddressList &Node::localAddresses()
{
	OC_METHODGATE();
	return mAddresses;
}



void Node::startComms()
{
	OC_METHODGATE();
	const NetworkAddress addr=localAddress();
	if(nullptr!=mComms) {
		qDebug()<<"comms.start  "<<addr.toString();
		mComms->start(addr);
	} else {
		qWarning()<<"ERROR: No comms";
	}
}

void Node::stopComms()
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		qDebug()<<"comms.stop "<<mComms->localID();
		mComms->stop();
	} else {
		qWarning()<<"ERROR: No comms";
	}
}

bool Node::isCommsStarted()
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		return mComms->isStarted();
	} else {
		qWarning()<<"ERROR: No comms";
	}
	return false;
}

bool Node::isCommsConnected()
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		return mComms->isConnected();
	} else {
		qWarning()<<"ERROR: No comms";
	}
	return false;
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


void Node::hookColorSignals(QObject &ob)
{
	OC_METHODGATE();
	if(nullptr!=mSensorsCourier) {
		if(!connect(&ob, SIGNAL(colorChanged(QColor)), mSensorsCourier, SLOT(onColorUpdated(QColor)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect " << ob.objectName();
		}
	}
}


void Node::unHookColorSignals(QObject &ob)
{
	OC_METHODGATE();
	if(nullptr!=mSensorsCourier) {
		if(!disconnect(&ob, SIGNAL(colorChanged(QColor)), mSensorsCourier, SLOT(onColorUpdated(QColor)))) {
			qWarning()<<"ERROR: Could not disconnect " << ob.objectName();
		}
	}
}



////////////////////////////////


void Node::hookSensorSignals(QObject &o)
{
	OC_METHODGATE();
	if(nullptr!=mSensors) {
		mSensors->hookSignals(o);
	}
}


void Node::unHookSensorSignals(QObject &o)
{
	OC_METHODGATE();
	if(nullptr!=mSensors) {
		mSensors->unHookSignals(o);
	}
}



void Node::setHookCommsSignals(QObject &o, bool hook)
{
	OC_METHODGATE();
	if(nullptr!=mComms) {
		mComms->setHookCommsSignals(o,hook);
	}
}



void Node::hookPeerSignals(QObject &o)
{
	OC_METHODGATE();
	mAssociates.hookSignals(o);
}


void Node::unHookPeerSignals(QObject &o)
{
	OC_METHODGATE();
	mAssociates.unHookSignals(o);
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

void Node::onCommsConnectionStatusChanged(bool s)
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
