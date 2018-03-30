#include "Agent.hpp"
#include "AgentWindow.hpp"

#include "utility/Standard.hpp"

#include "comms/CommsSession.hpp"
#include "comms/CommsSessionDirectory.hpp"
#include "comms/couriers/Courier.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"
#include "comms/SyncParameter.hpp"

#include "discovery/DiscoveryClient.hpp"
#include "zoo/ZooClient.hpp"
#include "node/AppContext.hpp"
#include "hw/controllers/ActuatorControllerFactory.hpp"
#include "basic/AddressEntry.hpp"

#include "AgentCourierSet.hpp"

#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
#include <QSharedPointer>



Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, mAgentConfigStore(mContext->baseDir() + "/agent_config.json")
	, mActuatorController(nullptr)
	, mKeyStoreReady(false)
	, mLocalIdentityStoreReady(false)
	, mAgentConfigStoreReady(false)

{
	OC_METHODGATE();
	// NOTE: Please do not put code here that generates events. Instead put them in init()
}

Agent::~Agent()
{
	OC_METHODGATE();
	if(nullptr!=mActuatorController) {
		mActuatorController->setConnected(false);
		mActuatorController=nullptr;
	}
}



void Agent::init()
{
	OC_METHODGATE();
	Node::init();
	mKeyStore.synchronize([this](SimpleDataStore &sms, bool ok) {
		qDebug()<<"Keystore synchronized with ok="<<ok;
		mKeyStoreReady=ok;
		checkLoadCompleted();
	});

	mLocalIdentity.synchronize([this](SimpleDataStore &sms, bool ok) {
		qDebug()<<"Local identity synchronized with ok="<<ok;
		mLocalIdentityStoreReady=ok;
		checkLoadCompleted();
	});

	mAgentConfigStore.synchronize([this](SimpleDataStore &sms, bool ok) {
		qDebug()<<"Agent Config Store synchronized with ok="<<ok;
		mAgentConfigStoreReady=ok;
		checkLoadCompleted();
		reloadController();
	});
}


void Agent::deInit()
{
	OC_METHODGATE();
	Node::deInit();
}


QSharedPointer<QWidget> Agent::showWindow()
{
	OC_METHODGATE();
	if(mWindow.isNull()) {
		QSharedPointer<Agent> sp=this->QEnableSharedFromThis<Agent>::sharedFromThis();
		if(sp.isNull()) {
			volatile int ba=0;
			qWarning()<<"SHARED POINTER TO THIS WAS NULL!"<<ba;
		}
		mWindow=QSharedPointer<AgentWindow>(OC_NEW AgentWindow(sp, nullptr));
	}
	if(!mWindow.isNull()) {
		mWindow->show();
	}
	return mWindow;
}



void Agent::setPanic(bool panic)
{
	OC_METHODGATE();
}



AgentConfigStore &Agent::configurationStore()
{
	OC_METHODGATE();
	return mAgentConfigStore;
}

QSharedPointer<PoseMapping> Agent::poseMapping()
{
	OC_METHODGATE();
	return mAgentConfigStore.agentConfig()->poseMapping();
}

IActuatorController *Agent::actuatorController()
{
	OC_METHODGATE();
	return mActuatorController;
}

void Agent::reloadController()
{
	OC_METHODGATE();
	// Unload old controller
	if(nullptr!=mActuatorController) {
		qDebug()<<"Unloading old actuator controller";
		mActuatorController->setConnected(false);
		mActuatorController->deleteLater();
		mActuatorController=nullptr;
	}
	QSharedPointer<AgentConfig> configp=mAgentConfigStore.agentConfig();
	if(!configp.isNull()) {
		AgentConfig &config=*configp;
		QString controllerName=config.controllerName().trimmed();
		if(!controllerName.isEmpty()) {
			qDebug()<<"Attempting to generate actuator controller of type "<<controllerName;
			ActuatorControllerFactory factory;
			mActuatorController=factory.controllerFactory(controllerName);
			if(nullptr!=mActuatorController) {
				qDebug()<<"Actuator controller created, configuring";
				QVariantMap controllerConfig=config.controllerConfig();
				mActuatorController->setConfiguration(controllerConfig);
				mActuatorController->setConnected(true);
			}
		} else {
			qDebug()<<"No actuator controller named in agent config";
		}
	} else {
		qDebug()<<"No agent config";
	}
}


void Agent::identityChanged()
{
	OC_METHODGATE();

	if(!mWindow.isNull()) {
		QSharedPointer<Agent> sp=this->QEnableSharedFromThis<Agent>::sharedFromThis();
		if(sp.isNull()) {
			volatile int ba=0;
			qWarning()<<"SHARED POINTER TO THIS WAS NULL!"<<ba;
		}
		mWindow->configure(sp);
	}
}


void Agent::setNodeCouriersRegistration(bool reg)
{
	OC_METHODGATE();
	Node::setNodeCouriersRegistration(reg);
	// When we get a new agent specific courier, put it here
	/*
	if(nullptr!=mComms){
		if(nullptr!=mAgentCourierX){
			mComms->setCourierRegistered(*mAgentCourierX, reg);
		}

	}*/
}



QSharedPointer<Node> Agent::sharedThis()
{
	OC_METHODGATE();
	return sharedFromThis();
}

bool Agent::checkLoadCompleted()
{
	OC_METHODGATE();
	const bool loaded = mKeyStoreReady && mLocalIdentityStoreReady && mAgentConfigStoreReady;
	qDebug()<<"CHECK LOAD COMPLETE: "<<loaded;
	if(loaded) {
		qDebug()<<"EMITTING LOAD COMLPETE";
		emit appLoaded();
	}
	return loaded;
}






//////////////////////////////////////////////////
// Agent State Courier slots

void Agent::onSyncParameterChanged(ISyncParameter *sp)
{
	OC_METHODGATE();
	//qDebug()<<"Agent ASC changed: "<<sp->toString();
	if(nullptr!=sp) {
		if("TargetPose"==sp->name()) {
			SyncParameter<Pose> *targetPoseParameter=(SyncParameter<Pose> *)sp;
			Pose targetPose=targetPoseParameter->bestValue(true);
			qDebug()<<"TARGET POSE: "<<targetPose.toString();
			if(nullptr!=mActuatorController) {
				mActuatorController->move(targetPose);
			}
		}
	} else {
		qWarning()<<"ERROR: sp was nullptr";
	}
}
