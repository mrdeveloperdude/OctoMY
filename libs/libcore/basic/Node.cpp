#include "Node.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/Client.hpp"

#include "comms/discovery/DiscoveryClient.hpp"

#include "zoo/ZooClient.hpp"
#include "camera/CameraList.hpp"
#include "sensory/SensorInput.hpp"
#include "comms/messages/SensorsMessage.hpp"
#include "utility/Utility.hpp"
#include "utility/ScopedTimer.hpp"
#include "basic/StyleManager.hpp"

#include <QCommandLineParser>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QStandardPaths>
#include <QDir>

Node::Node(QCommandLineParser &opts, QString base, DiscoveryRole role, DiscoveryType type, QObject *parent)
	: QObject (parent)
	, opts (opts)
	, settings(nullptr, base)
	, baseDir( settings.getCustomSetting("content_dir", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)) )
	, keystore (nullptr, baseDir + "/keystore.json")
	, peers (nullptr, baseDir + "/peers.json")
	, discovery (new DiscoveryClient(*this))
	, role (role)
	, type (type)
	, comms (new CommsChannel(this))
	, zoo (new ZooClient(this))
	, sensors (new SensorInput(this))
	, hubPort (0)
	, cameras (new CameraList(this))
	, lastStatusSend (0)
	, sensorMessage (new SensorsMessage)
{
	ScopedTimer nodeBootTimer(base+"-boot");
	setObjectName(base);
	QCoreApplication::setApplicationVersion("1.0");
	QCoreApplication::setApplicationName(Settings::APPLICATION_NAME_BASE+" "+base);

	// Only Agents are "born"
	if(ROLE_AGENT!=role){
		keystore.bootstrap();
	}


	StyleManager *style=new StyleManager(QColor(TYPE_AGENT==type?"#e83636":TYPE_REMOTE==type?"#36bee8":"#36e843"));
	if(nullptr!=style){
		style->apply();
	}

	if(!QDir().mkpath(baseDir)){
		qWarning()<<"ERROR: Could not create basedir for node";
	}

//	discovery->configure(this);
	hookSensorSignals(*this);
	hookCommsSignals(*this);

	//QByteArray OCID=UniquePlatformFingerprint::getInstance().platform().getHEX().toUtf8();
	if(nullptr!=zoo){
		zoo->setURL(QUrl("http://localhost:8123/api"));
		//zoo->setURL(QUrl("http://localhost/lennart/octomy/index.php"));
		//zoo->putNode(OCID); 		zoo->getNode(OCID);
	}

}

Node::~Node(){
	unHookSensorSignals(*this);
	unHookCommsSignals(*this);
	delete sensorMessage;
	sensorMessage=nullptr;

}

QCommandLineParser &Node::getOptions()
{
	return opts;
}

Settings &Node::getSettings()
{
	return settings;
}

KeyStore  &Node::getKeyStore()
{
	return keystore;
}

DiscoveryClientStore &Node::getPeers()
{
	return  peers;
}

DiscoveryClient *Node::getDiscoveryClient()
{
	return discovery;
}

DiscoveryRole Node::getRole()
{
	return role;
}


DiscoveryType Node::getType()
{
	return type;
}

CommsChannel *Node::getComms()
{
	return comms;
}

ZooClient *Node::getZooClient()
{
	return zoo;
}

SensorInput *Node::getSensorInput()
{
	return sensors;
}

CameraList *Node::getCameras()
{
	return cameras;
}

QWidget *Node::showWindow()
{
	return nullptr;
}


void Node::hookSensorSignals(QObject &o)
{
	if(nullptr!=sensors){
		sensors->hookSignals(o);
	}
}


void Node::unHookSensorSignals(QObject &o)
{
	if(nullptr!=sensors){
		sensors->unHookSignals(o);
	}
}



void Node::hookCommsSignals(QObject &o)
{
	if(nullptr!=comms){
		comms->hookSignals(o);
	}
}


void Node::unHookCommsSignals(QObject &o)
{
	if(nullptr!=comms){
		comms->unHookSignals(o);
	}
}




void Node::sendStatus(){
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	const qint64 interval=now-lastStatusSend;
	if(interval>100){
		QByteArray datagram;
		QDataStream ds(&datagram,QIODevice::WriteOnly);
		ds<<sensorMessage;
		//TODO:Convert to use courier instead
		//		comms->sendPackage(datagram,QHostAddress(hubAddress),hubPort);
		lastStatusSend=now;
	}
}



//////////////////////////////////////////////////
//CommsChannel slots


void Node::onError(QString e){
	qDebug()<<"Comms error: "<<e;
}

void Node::onClientAdded(Client *c){
	qDebug()<<"Client added: "<<c->getHash() <<QString::number(c->getHash(),16);
}

void Node::onConnectionStatusChanged(bool s){
	qDebug() <<"New connection status: "<<s;
	if(s){

	}
	else{

	}
}



//////////////////////////////////////////////////
//Internal sensor slots

void Node::onPositionUpdated(const QGeoPositionInfo &info){
	if(nullptr!=sensorMessage){
		sensorMessage->gps=info.coordinate();
		sendStatus();
	}
}


void Node::onCompassUpdated(QCompassReading *r){
	if(nullptr!=r && nullptr!=sensorMessage){
		sensorMessage->compassAzimuth=r->azimuth();
		sensorMessage->compassAccuracy=r->calibrationLevel();
		sendStatus();
	}
}

void Node::onAccelerometerUpdated(QAccelerometerReading *r){
	if(nullptr!=r && nullptr!=sensorMessage){
		sensorMessage->accellerometer=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
}

void Node::onGyroscopeUpdated(QGyroscopeReading *r){
	if(nullptr!=r && nullptr!=sensorMessage){
		sensorMessage->gyroscope=QVector3D(r->x(), r->y(), r->z());
		sendStatus();
	}
}
