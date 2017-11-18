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

#include "AgentCourierSet.hpp"

#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>



Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(OC_NEW AppContext(launcher.options(), launcher.environment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, mAgentConfigStore(mContext->baseDir() + "/agent_config.json")
	, mActuatorController(nullptr)
	, mWindow(nullptr)
	, mThis(this)
{
	OC_METHODGATE();
	mAddressBook.hookSignals(*this);
	if(mAddressBook.isReady()) {
		onAddressBookReady(true);
	}
	mAgentConfigStore.bootstrap(true, false);
}

Agent::~Agent()
{
	OC_METHODGATE();
	if(nullptr!=mActuatorController) {
		mActuatorController->setConnected(false);
	}
}




QWidget *Agent::showWindow()
{
	OC_METHODGATE();
	if(nullptr==mWindow) {
		mWindow=OC_NEW AgentWindow(mThis, nullptr);
	}
	if(nullptr!=mWindow) {
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
	return mAgentConfigStore.agentConfig()->poseMapping();
}

IActuatorController *Agent::actuatorController()
{
	return mActuatorController;
}

void Agent::reloadController()
{
	// Unload old controller
	if(nullptr!=mActuatorController) {
		qDebug()<<"Unloading old controller";
		mActuatorController->setConnected(false);
		mActuatorController->deleteLater();
		mActuatorController=nullptr;
	}
	QSharedPointer<AgentConfig> configp=mAgentConfigStore.agentConfig();
	if(!configp.isNull()) {
		AgentConfig &config=*configp;
		QString controllerName=config.controllerName().trimmed();
		if(!controllerName.isEmpty()) {
			qDebug()<<"Attempting to generate controller of type "<<controllerName;
			ActuatorControllerFactory factory;
			mActuatorController=factory.controllerFactory(controllerName);
			if(nullptr!=mActuatorController) {
				qDebug()<<"Controller created, configuring";
				QVariantMap controllerConfig=config.controllerConfig();
				mActuatorController->setConfiguration(controllerConfig);
				mActuatorController->setConnected(true);
			}
		} else {
			qDebug()<<"No controller named in agent config";
		}
	} else {
		qDebug()<<"No agent config";
	}
}



void Agent::setNodeCouriersRegistered(bool reg){
	OC_METHODGATE();
	Node::setNodeCouriersRegistered(reg);
	// When we get a new agent specific courier, put it here
	/*
	if(nullptr!=mComms){
		if(nullptr!=mAgentCourierX){
			mComms->setCourierRegistered(*mAgentCourierX, reg);
		}

	}*/
}


//////////////////////////////////////////////////
// Agent Config Store slots



void Agent::onAgentConfigStoreReady(bool ok)
{
	qDebug()<<"Agent config store ready";
	reloadController();
}

#include "basic/AddressEntry.hpp"
#include <QSharedPointer>

//////////////////////////////////////////////////
// Node Associate Store slots
void Agent::onAddressBookReady(bool ready)
{
	OC_METHODGATE();
}



void Agent::onAssociateAdded(QString)
{
	OC_METHODGATE();
}

void Agent::onAssociateRemoved(QString)
{
	OC_METHODGATE();
}

void Agent::onAssociateChanged()
{
	OC_METHODGATE();
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


//////////////////////////////////////////////////
// CommsChannel slots


void Agent::onCommsError(QString e)
{
	OC_METHODGATE();
	//qDebug()<<"AGENT UNIMP Comms error: "<<e;
}

void Agent::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	//qDebug()<<"AGENT UNIMP Client added: "<<c->toString();
}

void Agent::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)
{
	OC_METHODGATE();
	//qDebug() <<"AGENT UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}
