#ifndef AGENTCLIENTCOURIERSET_HPP
#define AGENTCLIENTCOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QDataStream>


class AgentClient;
class BlobCourier;
class SensorsCourier;
class AgentStateCourier;


class AgentClientCourierSet : public CourierSet
{
private:
	QSharedPointer<AgentClient> mClient;
	QDataStream mDatastream;
	QSharedPointer<AgentStateCourier> mAgentStateCourier;
	QSharedPointer<SensorsCourier> mSensorsCourier;
	QSharedPointer<BlobCourier> mBlobCourier;

public:
	explicit AgentClientCourierSet();
	virtual ~AgentClientCourierSet();

public:
	void configure(QString fullID, QSharedPointer<AgentClient> client);

public:
	QSharedPointer<AgentStateCourier> agentStateCourier();

};

#endif
// AGENTCLIENTCOURIERSET_HPP
