#include "DiscoveryClientService.hpp"

#include "node/Node.hpp"


DiscoveryClientService::DiscoveryClientService(QSharedPointer<DiscoveryClient> discoveryClient, QStringList dependencies)
	: ServiceWrapper<DiscoveryClient>(discoveryClient, "DiscoveryClient", dependencies)
	, mConfigureHelper("DiscoveryClientService", true, true, false, true, false)
{
	OC_METHODGATE();
}


void DiscoveryClientService::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
	}
}

void DiscoveryClientService::serviceWrapperActivate(QSharedPointer<DiscoveryClient> discoveryClient, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		discoveryClient->activate(on);
		if(on) {
			if(!mNode.isNull()) {
				discoveryClient->setURL(mNode->serverURL());
			} else {
				qWarning()<<"ERROR: No node";
			}
			if(nullptr!=callBack) {
				callBack(true);
			}

		}
	}
}



