#ifndef REMOTECOURIERSET_HPP
#define REMOTECOURIERSET_HPP



#include "comms/couriers/CourierSet.hpp"


#include <QDataStream>
#include <QSharedPointer>

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
	RemoteClient & mRemoteClient;

public:
	explicit RemoteCourierSet(QString fullID, RemoteClient &cl);
	virtual ~RemoteCourierSet();

public:

	QSharedPointer<AgentStateCourier> agentStateCourier();
};


#endif // REMOTECOURIERSET_HPP






