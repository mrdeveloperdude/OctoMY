#include "CourierSet.hpp"

#include "utility/Standard.hpp"

#include "comms/couriers/Courier.hpp"
#include "comms/CommsChannel.hpp"


CourierSet::CourierSet()

{
	OC_METHODGATE();
}

CourierSet::~CourierSet()
{
	OC_METHODGATE();
	for(Courier *courier: *this) {
		courier->deleteLater();
	}
	clear();
}


void CourierSet::setCommsEnabled(bool enable)
{
	OC_METHODGATE();
	for(Courier *courier: *this) {
		if(nullptr!=courier) {
			CommsChannel &cc=courier->comms();
			cc.setCourierRegistered(*courier, enable);
		}
	}
}

bool CourierSet::commsEnabled(bool conservative)
{
	OC_METHODGATE();
	int activeCount=0;
	for(Courier *courier: *this) {
		if(nullptr!=courier) {
			CommsChannel &cc=courier->comms();
			if(cc.hasCourier(*courier)) {
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
