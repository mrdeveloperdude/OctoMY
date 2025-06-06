#include "AgentControls.hpp"

#include "Agent.hpp"
#include "comms/couriers/sets/AgentCourierSet.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


AgentControls::AgentControls(Agent &agent)
	: mAgent(agent)
{
	OC_METHODGATE();
}

AgentControls::~AgentControls()
{
	OC_METHODGATE();
}


void AgentControls::registerClient(QString fullID)
{
	OC_METHODGATE();
	if(!mCouriers.contains(fullID) ) {
		AgentCourierSet *set=OC_NEW AgentCourierSet (fullID, mAgent);
		mCouriers.insert(fullID, set);
	} else {
		qWarning()<<"ERROR: Trying to register same client twice: "<<fullID;
	}
}

void AgentControls::unRegisterClient(QString fullID)
{
	OC_METHODGATE();
	if(mCouriers.contains(fullID) ) {
		mCouriers.remove(fullID);
	} else {
		qWarning()<<"ERROR: Trying to un-register nonexistant client: "<<fullID;
	}
}

void AgentControls::setCommsEnabled(bool enable)
{
	OC_METHODGATE();
	for(AgentCourierSet *set:mCouriers) {
		set->enable(enable);
	}
}

AgentCourierSet *AgentControls::activeControl() const
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


