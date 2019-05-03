#include "CarrierService.hpp"



CarrierService::CarrierService(QSharedPointer<CommsCarrier> carrier, QStringList dependencies)
	: ServiceWrapper<CommsCarrier>(carrier, "Carrier", dependencies)
{
	OC_METHODGATE();
}

void CarrierService::serviceWrapperActivate(QSharedPointer<CommsCarrier> carrier, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	carrier->activate(on);
	callBack(true);
}
