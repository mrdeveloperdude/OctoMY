#include "AgentControls.hpp"

#include "CourierSet.hpp"

#include "Agent.hpp"
#include "comms/CommsSignature.hpp"


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


