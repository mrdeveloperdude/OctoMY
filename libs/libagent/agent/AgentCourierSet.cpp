#include "AgentCourierSet.hpp"

#include "Agent.hpp"
#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/BlobCourier.hpp"


AgentCourierSet::AgentCourierSet(QString &fullID, Agent &agent)
	: CourierSet()
	, mAgent(agent)
	, mAgentStateCourier(new AgentStateCourier(&mDatastream,nullptr))
	, mSensorsCourier(new SensorsCourier(nullptr))
	, mBlobCourier(new BlobCourier(nullptr))
{
	OC_METHODGATE();
	if(nullptr!=mAgentStateCourier) {
		mAgentStateCourier->setHookSignals(mAgent, true);
		mAgentStateCourier->setDestination(fullID);
		append(mAgentStateCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate AgentStateCourier";
	}
	if(nullptr!=mSensorsCourier) {
		mSensorsCourier->setDestination(fullID);
		append(mSensorsCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate SensorsCourier";
	}
	if(nullptr!=mBlobCourier) {
		mBlobCourier->setDestination(fullID);
		append(mBlobCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate BlobCourier";
	}
}




AgentCourierSet::~AgentCourierSet()
{
	OC_METHODGATE();
	mAgentStateCourier=nullptr;
	mSensorsCourier=nullptr;
	mBlobCourier=nullptr;
}



AgentStateCourier *AgentCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}
