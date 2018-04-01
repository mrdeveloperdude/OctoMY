#ifndef AGENTCLIENTCOURIERSET_HPP
#define AGENTCLIENTCOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"


#include <QDataStream>
#include <QSharedPointer>

class AgentClient;

class AgentClientCourierSet : public CourierSet
{
private:
	AgentClient &mClient;
	QDataStream mDatastream;
	QSharedPointer<AgentStateCourier> mAgentStateCourier;
	QSharedPointer<SensorsCourier> mSensorsCourier;
	QSharedPointer<BlobCourier> mBlobCourier;

public:
	explicit AgentClientCourierSet(QString fullID, AgentClient &client);
	virtual ~AgentClientCourierSet();

public:

	QSharedPointer<AgentStateCourier> agentStateCourier();
};

#endif // AGENTCLIENTCOURIERSET_HPP
