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

#include <QList>

class CourierSet: public QList<Courier *>
{
private:


public:
	explicit CourierSet();
	virtual ~CourierSet();
public:
	void setCommsEnabled(bool enable);
};

#endif // COURIERSET_HPP
