#include "CourierSet.hpp"
#include "Agent.hpp"
#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/BlobCourier.hpp"

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
