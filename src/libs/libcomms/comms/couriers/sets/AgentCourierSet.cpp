#include "AgentCourierSet.hpp"

#include "agent/Agent.hpp"
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

	auto cc=mAgent.comms();
	if(nullptr!=cc) {
		mAgentStateCourier = QSharedPointer<AgentStateCourier>::create(nullptr, cc, &mAgent);
		mSensorsCourier = QSharedPointer<SensorsCourier>::create(cc, &mAgent);
		mBlobCourier = QSharedPointer<BlobCourier>::create(cc, &mAgent);
	} else {
		qWarning()<<"ERROR: AgentCourierSet did not have commschannel";
	}

	if(!mAgentStateCourier.isNull()) {
		mAgentStateCourier->setHookSignals(mAgent, true);
		mAgentStateCourier->setDestination(fullID);
		insert(mAgentStateCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate AgentStateCourier";
	}
	if(!mSensorsCourier.isNull()) {
		mSensorsCourier->setDestination(fullID);
		insert(mSensorsCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate SensorsCourier";
	}
	if(!mBlobCourier.isNull()) {
		mBlobCourier->setDestination(fullID);
		insert(mBlobCourier);
	} else {
		qWarning()<<"ERROR: Could not allocate BlobCourier";
	}
}


AgentCourierSet::~AgentCourierSet()
{
	OC_METHODGATE();
}


QSharedPointer<AgentStateCourier> AgentCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}
