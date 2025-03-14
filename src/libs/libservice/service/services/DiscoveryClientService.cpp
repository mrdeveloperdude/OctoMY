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
		mNode = node;
	}
}

void DiscoveryClientService::serviceWrapperActivate(QSharedPointer<DiscoveryClient> discoveryClient, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	bool ok = false;
	if(mConfigureHelper.activate(on)) {
		if(on) {
			if(!mNode.isNull()) {
				discoveryClient->setZooURL(mNode->serverURL());
				// qDebug()<<"SET URL: " << discoveryClient->URL();
				ok = true;
			} else {
				qWarning()<<"ERROR: No node";
			}
		}
		discoveryClient->activate(on);
	}
	if(nullptr != callBack) {
		callBack(on, ok);
	}
}



