#include "RemoteCourierSet.hpp"

#include "comms/CommsSignature.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "node/ClientWidget.hpp"


RemoteCourierSet::RemoteCourierSet(QString fullID, ClientWidget &cw)
	: CourierSet()
	, mAgentStateCourier(nullptr)
	, mSensorsCourier(nullptr)
	, mBlobCourier(nullptr)
{
	OC_METHODGATE();

	CommsChannel *cc=cw.comms();
	if(nullptr!=cc) {
		mAgentStateCourier=(new AgentStateCourier(nullptr , *cc, &cw));
		mSensorsCourier=(new SensorsCourier(*cc, &cw));
		mBlobCourier=(new BlobCourier(*cc, &cw));
	} else {
		qWarning()<<"ERROR: ClientWidget did not have commschannel";
	}


	if(nullptr!=mAgentStateCourier) {
		mAgentStateCourier->setHookSignals(cw, true);
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




RemoteCourierSet::~RemoteCourierSet()
{
	OC_METHODGATE();
	delete mAgentStateCourier;
	mAgentStateCourier=nullptr;
	delete mSensorsCourier;
	mSensorsCourier=nullptr;
	delete mBlobCourier;
	mBlobCourier=nullptr;
}



AgentStateCourier *RemoteCourierSet::agentStateCourier()
{
	OC_METHODGATE();
	return mAgentStateCourier;
}


