#ifndef HUBCOURIERSET_HPP
#define HUBCOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QDataStream>

class HubClient;


class AgentStateCourier;
class SensorsCourier;
class BlobCourier;

class HubCourierSet : public CourierSet
{
private:
	QSharedPointer<AgentStateCourier> mAgentStateCourier;
	QSharedPointer<SensorsCourier> mSensorsCourier;
	QSharedPointer<BlobCourier> mBlobCourier;
	HubClient & mHubClient;

public:
	explicit HubCourierSet(QString fullID, HubClient &cl);
	virtual ~HubCourierSet();

public:
	QSharedPointer<AgentStateCourier> agentStateCourier();

};


#endif
// HUBCOURIERSET_HPP
