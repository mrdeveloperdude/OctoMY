#include "CourierSet.hpp"

#include "utility/Standard.hpp"

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


void CourierSet::setCommsEnabled(bool enable)
{
	OC_METHODGATE();
	for(QSharedPointer<Courier> courier: *this) {
		if(!courier.isNull()) {
			CommsChannel &cc=courier->comms();
			cc.setCourierRegistered(courier, enable);
		}
	}
}

bool CourierSet::commsEnabled(bool conservative)
{
	OC_METHODGATE();
	int activeCount=0;
	for(QSharedPointer<Courier> courier: *this) {
		if(!courier.isNull()) {
			CommsChannel &cc=courier->comms();
			if(cc.couriers().contains(courier)) {
				activeCount++;
			}
		}
	}
	if(conservative) {
		return count()==activeCount;
	} else {
		return activeCount>0;
	}
}

bool  CourierSet::setRegistered(QSharedPointer<Courier> courier, bool reg)
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
