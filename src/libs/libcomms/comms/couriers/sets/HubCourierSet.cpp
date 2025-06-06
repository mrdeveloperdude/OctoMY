#include "HubCourierSet.hpp"

#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "client/hub/HubClient.hpp"
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

	QSharedPointer<Comms> cc=nullptr;
	if(!ctl.isNull()) {
		cc=ctl->comms();
	}

	if(!cc.isNull()) {
		mAgentStateCourier = QSharedPointer<AgentStateCourier>::create(nullptr , cc, &mHubClient);
		if(!mAgentStateCourier.isNull()) {
			mAgentStateCourier->setHookSignals(mHubClient, true);
			mAgentStateCourier->setDestination(fullID);
			insert(mAgentStateCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate AgentStateCourier";
		}
		mSensorsCourier = QSharedPointer<SensorsCourier>::create(cc, &mHubClient);
		if(!mSensorsCourier.isNull()) {
			mSensorsCourier->setDestination(fullID);
			insert(mSensorsCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate SensorsCourier";
		}
		mBlobCourier = QSharedPointer<BlobCourier>::create(cc, &mHubClient);
		if(!mBlobCourier.isNull()) {
			mBlobCourier->setDestination(fullID);
			insert(mBlobCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate BlobCourier";
		}
	} else {
		qWarning()<<"ERROR: HubCourierSet did not have commschannel";
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


