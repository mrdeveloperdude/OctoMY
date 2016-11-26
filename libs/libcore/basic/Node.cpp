#include "Node.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/Client.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "zoo/ZooClient.hpp"
#include "camera/CameraList.hpp"
#include "sensory/SensorInput.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/BlobCourier.hpp"
#include "../libutil/utility/Utility.hpp"
#include "../libutil/utility/ScopedTimer.hpp"
#include "basic/StyleManager.hpp"
#include "AppContext.hpp"
#include "zoo/ZooConstants.hpp"

#include <QCommandLineParser>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QDir>

Node::Node(AppContext *context, DiscoveryRole role, DiscoveryType type, QObject *parent)
	: QObject (parent)
	, mContext(context)
	, mKeystore (mContext->baseDir() + "/keystore.json")
	, mPeers (mContext->baseDir() + "/peers.json")
	, mDiscovery (new DiscoveryClient(*this))
	, mRole (role)
	, mType (type)
	, mComms (new CommsChannel(mKeystore.localKey().id(), nullptr, (QObject *)this))
	, mZooClient (new ZooClient(this))
	, mSensors (new SensorInput(this))
	, mSensorsCourier(new SensorsCourier(this))
	, mBlobCourier(new BlobCourier(this))
	  //, hubPort (0)
	, mCameras (new CameraList(this))
	, mLastStatusSend (0)
//	, mSensorMessage (new SensorsMessage)
	  //, mServerURL("http://zoo.octomy.org/api")
//	, mServerURL("http://"+utility::localAddress()+":"+QString::number(ZooConstants::OCTOMY_UDP_DEFAULT_PORT_ZOO)+"/api")
	, mServerURL("http://10.0.0.86:"+QString::number(ZooConstants::OCTOMY_UDP_DEFAULT_PORT_ZOO)+"/api") //Local address HQ
{
	//ScopedTimer nodeBootTimer(mContext->base()+"-boot");
	setObjectName(mContext->base());

	init();
}

Node::~Node()
{
	unHookSensorSignals(*this);
	unHookCommsSignals(*this);
}

void Node::init()
{

	mKeystore.hookSignals(*this);
	// Only Agents are "born"
	mKeystore.bootstrap(ROLE_AGENT==mRole);

	mPeers.bootstrap(true, false);

	StyleManager *style=new StyleManager(QColor(TYPE_AGENT==mType?"#e83636":TYPE_REMOTE==mType?"#36bee8":"#36e843"));
	if(nullptr!=style) {
		style->apply();
	}

	if(!QDir().mkpath(mContext->baseDir())) {
		qWarning()<<"ERROR: Could not create basedir for node";
	}

	hookSensorSignals(*this);
	hookSensorSignals(*mSensorsCourier);

	hookCommsSignals(*this);

	if(nullptr!=mZooClient) {
		mZooClient->setURL(mServerURL);
	}

	if(nullptr!=mDiscovery) {
		mDiscovery->setURL(mServerURL);
	}
}

const QCommandLineParser &Node::options() const
{
	return mContext->options();
}

Settings &Node::settings()
{
	return mContext->settings();
}

KeyStore  &Node::keyStore()
{
	return mKeystore;
}

NodeAssociateStore &Node::peers()
{
	return  mPeers;
}

DiscoveryClient *Node::discoveryClient()
{
	return mDiscovery;
}

DiscoveryRole Node::role()
{
	return mRole;
}


DiscoveryType Node::type()
{
	return mType;
}

QString Node::name()
{
	QSharedPointer<NodeAssociate>  me=nodeIdentity();
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
	return mComms;
}

ZooClient *Node::zooClient()
{
	return mZooClient;
}

SensorInput *Node::sensorInput()
{
	return mSensors;
}

QSharedPointer<NodeAssociate> Node::nodeIdentity()
{
	QSharedPointer<NodeAssociate> me=mPeers.getParticipant(mKeystore.localKey().id());
	return me;
}

CameraList *Node::cameras()
{
	return mCameras;
}

QWidget *Node::showWindow()
{
	return nullptr;
}



void Node::start(const NetworkAddress &localAddress)
{
	if(nullptr!=mComms) {
		qDebug()<<"comms.start  "<<localAddress.toString();
		mComms->start(localAddress);
	} else {
		qWarning()<<"ERROR: No comms";
	}
}

void Node::stop()
{
	if(nullptr!=mComms) {
		qDebug()<<"comms.stop "<<mComms->localSignature().address();
		mComms->stop();
	} else {
		qWarning()<<"ERROR: No comms";
	}
}


bool Node::isCommsConnected()
{
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
	if(nullptr!=mSensorsCourier) {
		if(!connect(&ob,SIGNAL(colorChanged(QColor)),mSensorsCourier,SLOT(onColorUpdated(QColor)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	}
}


void Node::unHookColorSignals(QObject &ob)
{
	if(nullptr!=mSensorsCourier) {
		if(!disconnect(&ob,SIGNAL(colorChanged(QColor)),mSensorsCourier,SLOT(onColorUpdated(QColor)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}



////////////////////////////////


void Node::hookSensorSignals(QObject &o)
{
	if(nullptr!=mSensors) {
		mSensors->hookSignals(o);
	}
}


void Node::unHookSensorSignals(QObject &o)
{
	if(nullptr!=mSensors) {
		mSensors->unHookSignals(o);
	}
}



void Node::hookCommsSignals(QObject &o)
{
	if(nullptr!=mComms) {
		mComms->hookSignals(o);
	}
}


void Node::unHookCommsSignals(QObject &o)
{
	if(nullptr!=mComms) {
		mComms->unHookSignals(o);
	}
}


void Node::hookPeerSignals(QObject &o)
{
	mPeers.hookSignals(o);
}


void Node::unHookPeerSignals(QObject &o)
{
	mPeers.unHookSignals(o);
}



void Node::updateDiscoveryClient()
{
	delete mDiscovery;
	mDiscovery=new DiscoveryClient(*this);
	mDiscovery->setURL(mServerURL);
}


//////////////////////////////////////////////////
// Key Store slots

void Node::onKeystoreReady(bool ok)
{
	if(ok && nullptr!= mComms) {
		mComms->setID(mKeystore.localKey().id());
	}
	//qDebug()<<"Key Store READY="<<mKeystore.isReady()<<", ERROR="<<mKeystore.hasError();
}


//////////////////////////////////////////////////
// CommsChannel slots


void Node::onCommsError(QString e)
{
	qDebug()<<"NODE UNIMP Comms error: "<<e;
}

void Node::onCommsClientAdded(Client *c)
{
	//qDebug()<<"NODE UNIMP Client added: "<<c->toString();
}

void Node::onCommsConnectionStatusChanged(bool s)
{
	//qDebug() <<"NODE UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}



//////////////////////////////////////////////////
// Internal sensor slots

void Node::onPositionUpdated(const QGeoPositionInfo &info)
{
}


void Node::onCompassUpdated(QCompassReading *r)
{
}

void Node::onAccelerometerUpdated(QAccelerometerReading *r)
{
}

void Node::onGyroscopeUpdated(QGyroscopeReading *r)
{
}
