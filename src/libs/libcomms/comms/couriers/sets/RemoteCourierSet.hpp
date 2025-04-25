#ifndef REMOTECOURIERSET_HPP
#define REMOTECOURIERSET_HPP

#include "comms/couriers/CourierSet.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QDataStream>


class RemoteClient;
class AgentStateCourier;
class SensorsCourier;
class BlobCourier;


class RemoteCourierSet : public CourierSet
{
private:
	QSharedPointer<AgentStateCourier> mAgentStateCourier;
	QSharedPointer<SensorsCourier> mSensorsCourier;
	QSharedPointer<BlobCourier> mBlobCourier;
	QSharedPointer<RemoteClient> mRemoteClient;

public:
	explicit RemoteCourierSet();
	virtual ~RemoteCourierSet();

public:
	void configure(QString fullID, QSharedPointer<RemoteClient> cl);


public:

	QSharedPointer<AgentStateCourier> agentStateCourier();
};


#endif
// REMOTECOURIERSET_HPP
