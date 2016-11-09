#include "Node.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/Client.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "zoo/ZooClient.hpp"
#include "camera/CameraList.hpp"
#include "sensory/SensorInput.hpp"
#include "comms/couriers/SensorsCourier.hpp"
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

	mComms->registerCourier(*mSensorsCourier);

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


void Node::onError(QString e)
{
	qDebug()<<"Comms error: "<<e;
}

void Node::onClientAdded(Client *c)
{
	qDebug()<<"Client added: "<<c->toString();
}

void Node::onConnectionStatusChanged(bool s)
{
	qDebug() <<"New connection status: "<<(s?"ONLINE":"OFFLINE");
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
