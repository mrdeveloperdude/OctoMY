#ifndef REMOTECOURIERSET_HPP
#define REMOTECOURIERSET_HPP



#include "comms/couriers/CourierSet.hpp"

#include <QDataStream>

class ClientWidget;

class RemoteCourierSet : public CourierSet
{
private:
	AgentStateCourier *mAgentStateCourier;
	SensorsCourier *mSensorsCourier;
	BlobCourier *mBlobCourier;

public:
	explicit RemoteCourierSet(QString fullID, ClientWidget &cw);
	virtual ~RemoteCourierSet();

public:

	AgentStateCourier *agentStateCourier();
};


#endif // REMOTECOURIERSET_HPP






