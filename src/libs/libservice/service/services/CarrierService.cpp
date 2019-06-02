#include "CarrierService.hpp"

#include "node/Node.hpp"

CarrierService::CarrierService(QSharedPointer<CommsCarrier> carrier, QStringList dependencies)
	: ServiceWrapper<CommsCarrier>(carrier, "Carrier", dependencies)
	, mConfigureHelper("CarrierService", true, true, false, true, false)
{
	OC_METHODGATE();
}


void CarrierService::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
	}
}


void CarrierService::serviceWrapperActivate(QSharedPointer<CommsCarrier> carrier, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	bool ok=false;
	if(mConfigureHelper.activate(on)) {
		if(nullptr!= carrier) {
			auto localAddressList=mNode->localAddressList();
			if(!localAddressList.isNull()) {
				const NetworkAddress listenAddress(QHostAddress::Any, localAddressList->port());
				carrier->setListenAddress(listenAddress);
				ok=carrier->activate(on);
			}
		}
	}
	if(nullptr!=callBack) {
		callBack(on, ok);
	}
}
