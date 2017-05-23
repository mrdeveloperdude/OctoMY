#include "CourierSet.hpp"

#include "utility/Standard.hpp"

#include "comms/couriers/Courier.hpp"

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
	/*
	CommsChannel *cc=mAgent.comms();
	if(nullptr!=cc) {
		for(Courier *courier:*this) {
			cc->setCourierRegistered(*courier, enable);
		}

	} else {
		qWarning()<<"ERROR: Could not register AgentSateCourier, no comms channel";
	}
	*/
}
