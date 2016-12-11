#include "Agent.hpp"
#include "AgentWindow.hpp"

#include "../libutil/utility/Standard.hpp"

#include "comms/Client.hpp"
#include "comms/ClientDirectory.hpp"
#include "comms/couriers/Courier.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/BlobCourier.hpp"
#include "comms/SyncParameter.hpp"

#include "discovery/DiscoveryClient.hpp"
#include "zoo/ZooClient.hpp"
#include "basic/AppContext.hpp"


#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>



CourierSet::CourierSet(ClientSignature &sig, Agent &agent)
	: mAgent(agent)
	, mAgentStateCourier(new AgentStateCourier(&mDS,nullptr))
	, mSensorsCourier(new SensorsCourier(nullptr))
	, mBlobCourier(new BlobCourier(nullptr))

{
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
	for(Courier *courier: *this) {
		courier->deleteLater();
	}
	clear();
}

AgentStateCourier *CourierSet::agentStateCourier()
{
	return mAgentStateCourier;
}


void CourierSet::setCommsEnabled(bool enable)
{
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
}

AgentControls::~AgentControls()
{

}


void AgentControls::registerClient(ClientSignature &sig)
{
	quint64 shid=sig.shortHandID();
	if(!mCouriers.contains(shid) ) {
		CourierSet *set=new CourierSet (sig,mAgent);
		mCouriers.insert(shid, set);
	} else {
		qWarning()<<"ERROR: Trying to register same client twice: "<<sig;
	}
}

void AgentControls::unRegisterClient(ClientSignature &sig)
{
	quint64 shid=sig.shortHandID();
	if(mCouriers.contains(shid) ) {
		mCouriers.remove(shid);
	} else {
		qWarning()<<"ERROR: Trying to un-register nonexistant client: "<<sig;
	}
}

void AgentControls::setCommsEnabled(bool enable)
{

	for(CourierSet *set:mCouriers) {
		set->setCommsEnabled(enable);
	}
}

CourierSet *AgentControls::activeControl() const
{
	//TODO: Manage which one is actually the ACTIVE one instead of just returning the first one
	return mCouriers.begin().value();
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(new AppContext(launcher.getOptions(), launcher.getEnvironment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, mControls(*this)
	, window(nullptr)
{

	mPeers.hookSignals(*this);
	if(mPeers.isReady()) {
		onPeerStoreReady(true);
	}
}

Agent::~Agent()
{

}




QWidget *Agent::showWindow()
{
	if(nullptr==window) {
		window=new AgentWindow(this, nullptr);
	}
	if(nullptr!=window) {
		window->show();
	}
	return window;
}



void Agent::setPanic(bool panic)
{
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
	return mControls;
}

//////////////////////////////////////////////////
//Node Associate Store slots
void Agent::onPeerStoreReady(bool ready)
{
	qDebug()<<"AGENT found peer store "<< (ready?"READY":"UNREADY");
	QMap<QString, QSharedPointer<NodeAssociate> > &peers=mPeers.getParticipants();
	for(QSharedPointer<NodeAssociate> peer:peers) {
		ClientSignature sig=peer->toClientSignature();
		if(sig.isValid() && ROLE_CONTROL==peer->role()) {
			qDebug()<<"Adding "<<sig.toString()<<"("<<peer->toString()<<") to agent control's list";
			mControls.registerClient(sig);
		}
	}
	mControls.setCommsEnabled(true);
}



void Agent::onPeerAdded(QString)
{

}

void Agent::onPeerRemoved(QString)
{

}

void Agent::onPeersChanged()
{

}



//////////////////////////////////////////////////
//Agent State Courier slots

void Agent::onSyncParameterChanged(ISyncParameter *sp)
{
	qDebug()<<"Agent ASC changed: "<<sp->toString();

}


//////////////////////////////////////////////////
// CommsChannel slots


void Agent::onCommsError(QString e)
{
	//qDebug()<<"AGENT UNIMP Comms error: "<<e;
}

void Agent::onCommsClientAdded(Client *c)
{
	//qDebug()<<"AGENT UNIMP Client added: "<<c->toString();
}

void Agent::onCommsConnectionStatusChanged(bool s)
{
	//qDebug() <<"AGENT UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}
