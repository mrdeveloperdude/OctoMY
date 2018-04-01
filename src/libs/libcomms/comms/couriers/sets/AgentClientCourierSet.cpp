#include "AgentClientCourierSet.hpp"

#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "node/AgentClient.hpp"

AgentClientCourierSet::AgentClientCourierSet(QString fullID, AgentClient &client)
	: CourierSet()
	, mClient(client)
	, mAgentStateCourier(nullptr)
	, mSensorsCourier(nullptr)
	, mBlobCourier(nullptr)
{
	OC_METHODGATE();

	CommsChannel *cc=nullptr;//mClient.comms();
	if(nullptr!=cc) {
		mAgentStateCourier=QSharedPointer<AgentStateCourier>(OC_NEW AgentStateCourier(nullptr , *cc, &mClient));
		mSensorsCourier=QSharedPointer<SensorsCourier>(OC_NEW SensorsCourier(*cc, &mClient));
		mBlobCourier=QSharedPointer<BlobCourier>(OC_NEW BlobCourier(*cc, &mClient));
	} else {
		qWarning()<<"ERROR: AgentClientCourierSet did not have commschannel";
	}

	if(!mAgentStateCourier.isNull()) {
		mAgentStateCourier->setHookSignals(mClient, true);
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




AgentClientCourierSet::~AgentClientCourierSet()
{
	OC_METHODGATE();
}



QSharedPointer<AgentStateCourier> AgentClientCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}
