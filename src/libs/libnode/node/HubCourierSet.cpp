#include "HubCourierSet.hpp"

#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "node/HubClient.hpp"
#include "node/Node.hpp"


HubCourierSet::HubCourierSet(QString fullID, HubClient &client)
	: CourierSet()
	, mAgentStateCourier(nullptr)
	, mSensorsCourier(nullptr)
	, mBlobCourier(nullptr)
	, mHubClient(client)
{
	OC_METHODGATE();

	auto ctl=mHubClient.node();

	CommsChannel *cc=nullptr;
	if(!ctl.isNull()) {
		//cc=ctl->com
	}

	if(nullptr!=cc) {
		mAgentStateCourier=QSharedPointer<AgentStateCourier>(OC_NEW AgentStateCourier(nullptr , *cc, &mHubClient));
		if(!mAgentStateCourier.isNull()) {
			mAgentStateCourier->setHookSignals(mHubClient, true);
			mAgentStateCourier->setDestination(fullID);
			insert(mAgentStateCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate AgentStateCourier";
		}
		mSensorsCourier=QSharedPointer<SensorsCourier>(OC_NEW SensorsCourier(*cc, &mHubClient));
		if(!mSensorsCourier.isNull()) {
			mSensorsCourier->setDestination(fullID);
			insert(mSensorsCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate SensorsCourier";
		}
		mBlobCourier=QSharedPointer<BlobCourier>(OC_NEW BlobCourier(*cc, &mHubClient));
		if(!mBlobCourier.isNull()) {
			mBlobCourier->setDestination(fullID);
			insert(mBlobCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate BlobCourier";
		}
	} else {
		qWarning()<<"ERROR: ClientWidget did not have commschannel";
	}
}




HubCourierSet::~HubCourierSet()
{
	OC_METHODGATE();
	// We rely on shared pointer to take care of the deletion of individual couriers
}



QSharedPointer<AgentStateCourier> HubCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}


