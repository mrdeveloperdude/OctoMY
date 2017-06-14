#include "AgentCourierSet.hpp"

#include "Agent.hpp"
#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"


AgentCourierSet::AgentCourierSet(QString &fullID, Agent &agent)
	: CourierSet()
	, mAgent(agent)
	, mAgentStateCourier(nullptr)
	, mSensorsCourier(nullptr)
	, mBlobCourier(nullptr)
{
	OC_METHODGATE();

	CommsChannel *cc=agent.comms();
	if(nullptr!=cc) {
		mAgentStateCourier=(new AgentStateCourier(nullptr , *cc, &agent));
		mSensorsCourier=(new SensorsCourier(*cc, &agent));
		mBlobCourier=(new BlobCourier(*cc, &agent));
	} else {
		qWarning()<<"ERROR: ClientWidget did not have commschannel";
	}

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
	delete mAgentStateCourier;
	mAgentStateCourier=nullptr;
	delete mSensorsCourier;
	mSensorsCourier=nullptr;
	delete mBlobCourier;
	mBlobCourier=nullptr;
}



AgentStateCourier *AgentCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}
