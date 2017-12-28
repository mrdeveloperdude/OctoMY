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
	: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, mAgentConfigStore(mContext->baseDir() + "/agent_config.json")
	, mActuatorController(nullptr)
{
	OC_METHODGATE();
	mKeyStore.setHookSignals(*this, true);
	if(mKeyStore.isReady()) {
		onKeystoreReady(mKeyStore.hasError());
	}
	mConfigStore.setHookSignals(*this, true);
	if(mConfigStore.isReady()) {
		onConfigReady(mConfigStore.hasError());
	}
	mAddressBook.setHookSignals(*this, true);
	if(mAddressBook.isReady()) {
		onAddressBookReady(mAddressBook.hasError());
	}
	mAgentConfigStore.bootstrap(true, false);
}

Agent::~Agent()
{
	OC_METHODGATE();
	mAddressBook.setHookSignals(*this, false);
	if(nullptr!=mActuatorController) {
		mActuatorController->setConnected(false);
		mActuatorController=nullptr;
	}
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
#define CHECK(A, B) const bool A = (B).isReady(); qDebug()<<"CHECK "  #B   " GAVE "<< A
	CHECK(adr, mAddressBook);
	CHECK(ag, mAgentConfigStore);
	CHECK(conf, mConfigStore);
	CHECK(key, mKeyStore);
#undef CHECK

	const bool loaded = adr && ag && conf && key ;

	qDebug()<<"CHECK LOAD COMPLETE: "<<loaded;
	if(loaded) {
		emit appLoaded();
	}
	return loaded;
}


//////////////////////////////////////////////////
// Key Store slots

void Agent::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	checkLoadCompleted();
}




//////////////////////////////////////////////////
// Config Store slots

void Agent::onConfigReady(bool ok)
{
	OC_METHODGATE();
	checkLoadCompleted();
}



//////////////////////////////////////////////////
// Agent Config Store slots



void Agent::onAgentConfigStoreReady(bool ok)
{
	OC_METHODGATE();
	qDebug()<<"Agent config store ready";
	checkLoadCompleted();
	reloadController();
}

#include "basic/AddressEntry.hpp"
#include <QSharedPointer>

//////////////////////////////////////////////////
// Node Associate Store slots
void Agent::onAddressBookReady(bool ready)
{
	OC_METHODGATE();
	checkLoadCompleted();
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
