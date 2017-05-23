#ifndef AGENTCOURIERSET_HPP
#define AGENTCOURIERSET_HPP

#include "CourierSet.hpp"

#include <QDataStream>

class AgentCourierSet : public CourierSet
{
private:
	Agent &mAgent;
	QDataStream mDatastream;
	AgentStateCourier *mAgentStateCourier;
	SensorsCourier *mSensorsCourier;
	BlobCourier *mBlobCourier;

public:
	explicit AgentCourierSet(QString &fullID, Agent &agent);
	virtual ~AgentCourierSet();

public:

	AgentStateCourier *agentStateCourier();
};

#endif // AGENTCOURIERSET_HPP
