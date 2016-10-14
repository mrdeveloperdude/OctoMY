#include "Node.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/Client.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "zoo/ZooClient.hpp"
#include "camera/CameraList.hpp"
#include "sensory/SensorInput.hpp"
#include "comms/messages/SensorsMessage.hpp"
#include "../libutil/utility/Utility.hpp"
#include "../libutil/utility/ScopedTimer.hpp"
#include "basic/StyleManager.hpp"
#include "AppContext.hpp"


#include <QCommandLineParser>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QDir>

ClientSignature sig;

Node::Node(AppContext *context, DiscoveryRole role, DiscoveryType type, QObject *parent)
	: QObject (parent)
	, mContext(context)
	, mKeystore (mContext->baseDir() + "/keystore.json")
	, mPeers (mContext->baseDir() + "/peers.json")
	, mDiscovery (new DiscoveryClient(*this))
	, mRole (role)
	, mType (type)
	, mComms (new CommsChannel(sig, nullptr, (QObject *)this))
	, mZooClient (new ZooClient(this))
	, mSensors (new SensorInput(this))
	//, hubPort (0)
	, mCameras (new CameraList(this))
	, mLastStatusSend (0)
	, mSensorMessage (new SensorsMessage)
	, mServerURL("http://10.0.0.86:8123/api") //katana wlan iface)
{
	//ScopedTimer nodeBootTimer(mContext->base()+"-boot");
	setObjectName(mContext->base());

	if(!connect(&mKeystore, &KeyStore::storeReady, this, &Node::onKeystoreReady)){
		qWarning()<<"ERROR: Could not connect "<<mKeystore.objectName();
	}
	else{
		//qDebug()<<"CONNECTED NODE TO KEYSTORE";
	}

	// Only Agents are "born"
	mKeystore.bootstrap(ROLE_AGENT==role);

	mPeers.bootstrap(true, false);

	StyleManager *style=new StyleManager(QColor(TYPE_AGENT==type?"#e83636":TYPE_REMOTE==type?"#36bee8":"#36e843"));
	if(nullptr!=style){
		style->apply();
	}

	if(!QDir().mkpath(mContext->baseDir())){
		qWarning()<<"ERROR: Could not create basedir for node";
	}

	hookSensorSignals(*this);
	hookCommsSignals(*this);

	//QByteArray OCID=UniquePlatformFingerprint::getInstance().platform().getHEX().toUtf8();


	if(nullptr!=mZooClient){
		mZooClient->setURL(mServerURL);
	}

	if(nullptr!=mDiscovery){
		mDiscovery->setURL(mServerURL);
	}

}

Node::~Node(){
	unHookSensorSignals(*this);
	unHookCommsSignals(*this);
	delete mSensorMessage;
	mSensorMessage=nullptr;

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
	QSharedPointer<NodeAssociate>  me=localNodeAssociate();
	QString name;
	if(nullptr!=me){
		name=me->name();
	}
	if(""!=name.trimmed()){
		switch(mType){
			case(TYPE_AGENT):{
					name="Agent ";
				}break;
			case(TYPE_HUB):{
					name="Hub ";
				}break;
			case(TYPE_REMOTE):{
					name="Remote ";
				}break;
			case(TYPE_ZOO):{
					name="Zoo ";
				}break;
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

QSharedPointer<NodeAssociate> Node::localNodeAssociate()
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


void Node::hookSensorSignals(QObject &o)
{
	if(nullptr!=mSensors){
		mSensors->hookSignals(o);
	}
}


void Node::unHookSensorSignals(QObject &o)
{
	if(nullptr!=mSensors){
		mSensors->unHookSignals(o);
	}
}



void Node::hookCommsSignals(QObject &o)
{
	if(nullptr!=mComms){
		mComms->hookSignals(o);
	}
}


void Node::unHookCommsSignals(QObject &o)
{
	if(nullptr!=mComms){
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



void Node::updateDiscoveryClient(){
	delete mDiscovery;
	mDiscovery=new DiscoveryClient(*this);
	mDiscovery->setURL(mServerURL);
}


void Node::sendStatus(){
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	const qint64 interval=now-mLastStatusSend;
	if(interval>100){
		QByteArray datagram;
		QDataStream ds(&datagram,QIODevice::WriteOnly);
		ds<<mSensorMessage;
		//TODO:Convert to use courier instead
		//		comms->sendPackage(datagram,QHostAddress(hubAddress),hubPort);
		mLastStatusSend=now;
	}
}

//////////////////////////////////////////////////
// Key Store slots

void Node::onKeystoreReady(bool ok)
{
	//qDebug()<<"Key Store READY="<<mKeystore.isReady()<<", ERROR="<<mKeystore.hasError();
}


//////////////////////////////////////////////////
// CommsChannel slots


void Node::onError(QString e){
	qDebug()<<"Comms error: "<<e;
}

void Node::onClientAdded(Client *c){
	qDebug()<<"Client added: "<<c->toString();
}

void Node::onConnectionStatusChanged(bool s){
	qDebug() <<"New connection status: "<<s;
	if(s){

	}
	else{

	}
}



//////////////////////////////////////////////////
// Internal sensor slots

void Node::onPositionUpdated(const QGeoPositionInfo &info){
	if(nullptr!=mSensorMessage){
		mSensorMessage->gps=info.coordinate();
		sendStatus();
	}
}


void Node::onCompassUpdated(QCompassReading *r){
	if(nullptr!=r && nullptr!=mSensorMessage){
		mSensorMessage->compassAzimuth=r->azimuth();
		mSensorMessage->compassAccuracy=r->calibrationLevel();
		sendStatus();
	}
}

void Node::onAccelerometerUpdated(QAccelerometerReading *r){
	if(nullptr!=r && nullptr!=mSensorMessage){
		mSensorMessage->accellerometer=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
}

void Node::onGyroscopeUpdated(QGyroscopeReading *r){
	if(nullptr!=r && nullptr!=mSensorMessage){
		mSensorMessage->gyroscope=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
}
