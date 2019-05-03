#include "CommsService.hpp"

#include "comms/CommsChannel.hpp"

CommsService::CommsService(QSharedPointer<CommsChannel> comms, QStringList dependencies)
	: ServiceWrapper<CommsChannel>(comms, "Comms", dependencies)
{
	OC_METHODGATE();
}

void CommsService::serviceWrapperActivate(QSharedPointer<CommsChannel> comms, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	comms->activate(on);
	callBack(true);
}
