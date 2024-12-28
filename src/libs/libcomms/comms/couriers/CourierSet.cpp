#include "CourierSet.hpp"

#include "uptime/MethodGate.hpp"

#include "comms/couriers/Courier.hpp"
#include "comms/CommsChannel.hpp"


#define MAX_CONCURRENT_COURIERS (100)

CourierSet::CourierSet()
	: QSet()
{
	OC_METHODGATE();
}


CourierSet::~CourierSet()
{
	OC_METHODGATE();
}


void CourierSet::enable(bool enable)
{
	OC_METHODGATE();
	for(QSharedPointer<Courier> courier: *this) {
		if(!courier.isNull()) {
			QSharedPointer<CommsChannel> cc=courier->comms();
			if(!cc.isNull()) {
				cc->registerCourier(courier, enable);
			}
		}
	}
}


bool CourierSet::isEnabled(bool conservative)
{
	OC_METHODGATE();
	int activeCount=0;
	for(QSharedPointer<Courier> courier: *this) {
		if(!courier.isNull()) {
			QSharedPointer<CommsChannel> cc=courier->comms();
			if(!cc.isNull()) {
				if(cc->couriers().contains(courier)) {
					activeCount++;
				}
			}
		}
	}
	if(conservative) {
		return count()==activeCount;
	} else {
		return activeCount>0;
	}
}


bool  CourierSet::registerCourier(QSharedPointer<Courier> courier, bool reg)
{
	OC_METHODGATE();
	if(!courier.isNull()) {
		if(reg) {
			if(contains(courier)) {
				qWarning()<<"WARNING: courier was allready registered";
				return false;
			} else if(size()>MAX_CONCURRENT_COURIERS) {
				qWarning()<<"ERROR: more than maximum of "<<QString::number(MAX_CONCURRENT_COURIERS)<<" couriers, skipping registration of new one: "<<courier->id()<<courier->name();
				return false;
			}
			insert(courier);
			//qDebug()<<"Registered courier: "<<c.id()<<c.name()<<" for a total of "<<mCouriers.size()<<" couriers";
		} else {
			if(!contains(courier)) {
				qDebug()<<"WARNING: courier was not registered while unregistering";
				return false;
			}
			remove(courier);
		}
		return true;
	}
	return false;
}
