#include "AgentClientCourierSet.hpp"

#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "client/agent/AgentClient.hpp"

AgentClientCourierSet::AgentClientCourierSet()
	: CourierSet()
{
	OC_METHODGATE();

}


AgentClientCourierSet::~AgentClientCourierSet()
{
	OC_METHODGATE();
}


void AgentClientCourierSet::configure(QString fullID, QSharedPointer<AgentClient> client)
{
	OC_METHODGATE();
	mClient=client;
	if(!mClient.isNull()) {
		QSharedPointer<CommsChannel> cc=mClient->comms();
		if(!cc.isNull()) {
			mAgentStateCourier=QSharedPointer<AgentStateCourier>(OC_NEW AgentStateCourier(&mDatastream, cc));
			mSensorsCourier=QSharedPointer<SensorsCourier>(OC_NEW SensorsCourier(cc));
			mBlobCourier=QSharedPointer<BlobCourier>(OC_NEW BlobCourier(cc));
		} else {
			qWarning()<<"ERROR: AgentClientCourierSet did not have commschannel";
		}

		if(!mAgentStateCourier.isNull()) {
			mAgentStateCourier->setHookSignals(*mClient, true);
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
	} else {
		qWarning()<<"ERROR: No client";
	}
}

QSharedPointer<AgentStateCourier> AgentClientCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}
