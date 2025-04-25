#ifndef AGENTCOURIERSET_HPP
#define AGENTCOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QDataStream>

class Agent;
class AgentStateCourier;
class SensorsCourier;
class BlobCourier;


class AgentCourierSet : public CourierSet
{
private:
	Agent &mAgent;
	QDataStream mDatastream;
	QSharedPointer<AgentStateCourier> mAgentStateCourier;
	QSharedPointer<SensorsCourier> mSensorsCourier;
	QSharedPointer<BlobCourier> mBlobCourier;

public:
	explicit AgentCourierSet(QString &fullID, Agent &agent);
	virtual ~AgentCourierSet();

public:
	QSharedPointer<AgentStateCourier> agentStateCourier();

};

#endif
// AGENTCOURIERSET_HPP
