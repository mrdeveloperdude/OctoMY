#include "RemoteCourierSet.hpp"

#include "client/remote/RemoteClient.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"
#include "node/Node.hpp"


RemoteCourierSet::RemoteCourierSet()
	: CourierSet()
{
	OC_METHODGATE();
}


RemoteCourierSet::~RemoteCourierSet()
{
	OC_METHODGATE();
	// NOTE: We rely on shared pointer to take care of the deletion of individual couriers
}

void RemoteCourierSet::configure(QString fullID, QSharedPointer<RemoteClient> client)
{
	OC_METHODGATE();
	mRemoteClient=client;
	if(!mRemoteClient.isNull()) {
		QSharedPointer<Node> node=mRemoteClient->node();
		QSharedPointer<Comms> cc;
		if(!node.isNull()) {
			cc=node->comms();
		}

		if(!cc.isNull()) {
			auto remoteClientPtr=mRemoteClient.data();
			mAgentStateCourier = QSharedPointer<AgentStateCourier>::create(nullptr, cc, remoteClientPtr);
			if(!mAgentStateCourier.isNull()) {
				mAgentStateCourier->setHookSignals(*remoteClientPtr, true);
				mAgentStateCourier->setDestination(fullID);
				insert(mAgentStateCourier);
			} else {
				qWarning()<<"ERROR: Could not allocate AgentStateCourier";
			}
			mSensorsCourier = QSharedPointer<SensorsCourier>::create(cc, remoteClientPtr);
			if(!mSensorsCourier.isNull()) {
				mSensorsCourier->setDestination(fullID);
				insert(mSensorsCourier);
			} else {
				qWarning()<<"ERROR: Could not allocate SensorsCourier";
			}
			mBlobCourier = QSharedPointer<BlobCourier>::create(cc, remoteClientPtr);
			if(!mBlobCourier.isNull()) {
				mBlobCourier->setDestination(fullID);
				insert(mBlobCourier);
			} else {
				qWarning()<<"ERROR: Could not allocate BlobCourier";
			}
		} else {
			qWarning()<<"ERROR: RemoteCourierSet did not have commschannel";
		}
	} else {
		qWarning()<<"ERROR: RemoteCourierSet did not have node";
	}
}


QSharedPointer<AgentStateCourier> RemoteCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}


