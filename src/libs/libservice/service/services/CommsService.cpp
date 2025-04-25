#include "CommsService.hpp"

#include "comms/Comms.hpp"

CommsService::CommsService(QSharedPointer<Comms> comms, QStringList dependencies)
	: ServiceWrapper<Comms>(comms, "Comms", dependencies)
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

void CommsService::serviceWrapperActivate(QSharedPointer<Comms> comms, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		comms->activate(on);
		callBack(on, true);
	}
}
