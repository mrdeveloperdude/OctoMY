#ifndef COURIERSET_HPP
#define COURIERSET_HPP


class ZooClient;
class DiscoveryClient;
class AgentWindow;
class AgentStateCourier;
class ISyncParameter;
class Agent;
class SensorsCourier;
class BlobCourier;
class CommsSignature;
class Courier;

#include <QDataStream>

class CourierSet: public QList<Courier *>
{
private:
	Agent &mAgent;
	QDataStream mDatastream;
	AgentStateCourier *mAgentStateCourier;
	SensorsCourier *mSensorsCourier;
	BlobCourier *mBlobCourier;

public:
	explicit CourierSet(QString &fullID, Agent &agent);
	virtual ~CourierSet();
public:

	AgentStateCourier *agentStateCourier();

	void setCommsEnabled(bool enable);
};

#endif // COURIERSET_HPP
