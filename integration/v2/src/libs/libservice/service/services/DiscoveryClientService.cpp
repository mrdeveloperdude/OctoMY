#include "DiscoveryClientService.hpp"



DiscoveryClientService::DiscoveryClientService(QSharedPointer<DiscoveryClient> discoveryClient, QStringList dependencies)
	: ServiceWrapper<DiscoveryClient>(discoveryClient, "DiscoveryClient", dependencies)
{
	OC_METHODGATE();
}

void DiscoveryClientService::serviceWrapperActivate(QSharedPointer<DiscoveryClient> discoveryClient, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	discoveryClient->activate(on);
	callBack(true);
}
