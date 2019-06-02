#include "RemoteCourierSet.hpp"

#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "node/RemoteClient.hpp"
#include "node/Node.hpp"


RemoteCourierSet::RemoteCourierSet(QString fullID, RemoteClient &client)
	: CourierSet()
	, mAgentStateCourier(nullptr)
	, mSensorsCourier(nullptr)
	, mBlobCourier(nullptr)
	, mRemoteClient(client)
{
	OC_METHODGATE();

	QSharedPointer<Node> node=mRemoteClient.node();

	CommsChannel *cc=nullptr;
	if(!node.isNull()) {
		cc=node->comms();
	}

	if(nullptr!=cc) {
		mAgentStateCourier=QSharedPointer<AgentStateCourier>(OC_NEW AgentStateCourier(nullptr , *cc, &mRemoteClient));
		if(!mAgentStateCourier.isNull()) {
			mAgentStateCourier->setHookSignals(mRemoteClient, true);
			mAgentStateCourier->setDestination(fullID);
			insert(mAgentStateCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate AgentStateCourier";
		}
		mSensorsCourier=QSharedPointer<SensorsCourier>(OC_NEW SensorsCourier(*cc, &mRemoteClient));
		if(!mSensorsCourier.isNull()) {
			mSensorsCourier->setDestination(fullID);
			insert(mSensorsCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate SensorsCourier";
		}
		mBlobCourier=QSharedPointer<BlobCourier>(OC_NEW BlobCourier(*cc, &mRemoteClient));
		if(!mBlobCourier.isNull()) {
			mBlobCourier->setDestination(fullID);
			insert(mBlobCourier);
		} else {
			qWarning()<<"ERROR: Could not allocate BlobCourier";
		}
	} else {
		qWarning()<<"ERROR: RemoteCourierSet did not have commschannel";
	}
}




RemoteCourierSet::~RemoteCourierSet()
{
	OC_METHODGATE();
	// We rely on shared pointer to take care of the deletion of individual couriers
}



QSharedPointer<AgentStateCourier> RemoteCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}


