#include "Agent.hpp"
#include "AgentWindow.hpp"

#include "../libutil/utility/Standard.hpp"

#include "comms/CommsSession.hpp"
#include "comms/CommsSessionDirectory.hpp"
#include "comms/couriers/Courier.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/BlobCourier.hpp"
#include "comms/SyncParameter.hpp"

#include "discovery/DiscoveryClient.hpp"
#include "zoo/ZooClient.hpp"
#include "basic/AppContext.hpp"
#include "hw/controllers/ActuatorControllerFactory.hpp"

#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>



CourierSet::CourierSet(CommsSignature &sig, Agent &agent)
	: mAgent(agent)
	, mAgentStateCourier(new AgentStateCourier(&mDatastream,nullptr))
	, mSensorsCourier(new SensorsCourier(nullptr))
	, mBlobCourier(new BlobCourier(nullptr))
{
	OC_METHODGATE();
	if(nullptr!=mAgentStateCourier) {
		mAgentStateCourier->setHookSignals(mAgent, true);
		mAgentStateCourier->setDestination(sig);
		append(mAgentStateCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate AgentStateCourier";
	}
	if(nullptr!=mSensorsCourier) {
		mSensorsCourier->setDestination(sig);
		append(mSensorsCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate SensorsCourier";
	}
	if(nullptr!=mBlobCourier) {
		mBlobCourier->setDestination(sig);
		append(mBlobCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate BlobCourier";
	}
}

CourierSet::~CourierSet()
{
	OC_METHODGATE();
	for(Courier *courier: *this) {
		courier->deleteLater();
	}
	clear();
}

AgentStateCourier *CourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}


void CourierSet::setCommsEnabled(bool enable)
{
	OC_METHODGATE();
	CommsChannel *cc=mAgent.comms();
	if(nullptr!=cc) {
		for(Courier *courier:*this) {
			cc->setCourierRegistered(*courier, enable);
		}

	} else {
		qWarning()<<"ERROR: Could not register AgentSateCourier, no comms channel";
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



AgentControls::AgentControls(Agent &agent)
	: mAgent(agent)
{
	OC_METHODGATE();
}

AgentControls::~AgentControls()
{
	OC_METHODGATE();
}


void AgentControls::registerClient(CommsSignature &sig)
{
	OC_METHODGATE();
	quint64 shid=sig.shortHandID();
	if(!mCouriers.contains(shid) ) {
		CourierSet *set=new CourierSet (sig,mAgent);
		mCouriers.insert(shid, set);
	} else {
		qWarning()<<"ERROR: Trying to register same client twice: "<<sig;
	}
}

void AgentControls::unRegisterClient(CommsSignature &sig)
{
	OC_METHODGATE();
	quint64 shid=sig.shortHandID();
	if(mCouriers.contains(shid) ) {
		mCouriers.remove(shid);
	} else {
		qWarning()<<"ERROR: Trying to un-register nonexistant client: "<<sig;
	}
}

void AgentControls::setCommsEnabled(bool enable)
{
	OC_METHODGATE();
	for(CourierSet *set:mCouriers) {
		set->setCommsEnabled(enable);
	}
}

CourierSet *AgentControls::activeControl() const
{
	OC_METHODGATE();
	//TODO: Manage which one is actually the ACTIVE one instead of just returning the first one
	auto b=mCouriers.begin();
	auto e=mCouriers.end();
	if(b==e) {
		return nullptr;
	}
	return b.value();
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(new AppContext(launcher.getOptions(), launcher.getEnvironment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, mControls(*this)
	, mAgentConfigStore(mContext->baseDir() + "/agent_config.json")
	, mActuatorController(nullptr)
	, mWindow(nullptr)
{
	OC_METHODGATE();
	mPeers.hookSignals(*this);
	if(mPeers.isReady()) {
		onPeerStoreReady(true);
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
		mWindow=new AgentWindow(this, nullptr);
	}
	if(nullptr!=mWindow) {
		mWindow->show();
	}
	return mWindow;
}



void Agent::setPanic(bool panic)
{
	OC_METHODGATE();
	CourierSet *set=mControls.activeControl();

	if(nullptr!=set) {
		AgentStateCourier *asc=set->agentStateCourier();
		if(nullptr!=asc) {
			asc->setPanic(panic);
		} else {
			qWarning()<<"ERROR: No active control";
		}
	} else {
		qWarning()<<"ERROR: No set";
	}

}



const AgentControls &Agent::controls() const
{
	OC_METHODGATE();
	return mControls;
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

//////////////////////////////////////////////////
// Agent Config Store slots



void Agent::onAgentConfigStoreReady(bool ok)
{
	qDebug()<<"Agent config store ready";
	reloadController();
}



//////////////////////////////////////////////////
// Node Associate Store slots
void Agent::onPeerStoreReady(bool ready)
{
	OC_METHODGATE();
	qDebug()<<"AGENT found peer store "<< (ready?"READY":"UNREADY");
	QMap<QString, QSharedPointer<NodeAssociate> > &peers=mPeers.getParticipants();
	for(QSharedPointer<NodeAssociate> peer:peers) {
		CommsSignature sig=peer->toClientSignature();
		if(sig.isValid() && ROLE_CONTROL==peer->role()) {
			qDebug()<<"Adding "<<sig.toString()<<"("<<peer->toString()<<") to agent control's list";
			mControls.registerClient(sig);
		}
	}
	mControls.setCommsEnabled(true);
}



void Agent::onPeerAdded(QString)
{
	OC_METHODGATE();
}

void Agent::onPeerRemoved(QString)
{
	OC_METHODGATE();
}

void Agent::onPeersChanged()
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

void Agent::onCommsConnectionStatusChanged(bool s)
{
	OC_METHODGATE();
	//qDebug() <<"AGENT UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}
