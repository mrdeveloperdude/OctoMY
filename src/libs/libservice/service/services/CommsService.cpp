#include "CommsService.hpp"

#include "comms/CommsChannel.hpp"

CommsService::CommsService(QSharedPointer<CommsChannel> comms, QStringList dependencies)
	: ServiceWrapper<CommsChannel>(comms, "Comms", dependencies)
	, mConfigureHelper("CommsService", true, true, false, true, false)
{
	OC_METHODGATE();
}


void CommsService::configure(/*QSharedPointer<Node> node*/)
{
	OC_METHODGATE();

	if(mConfigureHelper.configure()) {
		//mNode=node;
	}

}

void CommsService::serviceWrapperActivate(QSharedPointer<CommsChannel> comms, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		comms->activate(on);
		callBack(on, true);
	}
}
