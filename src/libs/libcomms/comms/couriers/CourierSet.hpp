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

#include <QSet>
#include <QSharedPointer>

class CourierSet: public QSet<QSharedPointer<Courier> >
{
private:


public:
	explicit CourierSet();
	virtual ~CourierSet();
public:
	void setCommsEnabled(bool enable);

	bool commsEnabled(bool conservative=true);


	bool setRegistered(QSharedPointer<Courier> courier, bool reg);
};

#endif // COURIERSET_HPP
