#include "LocalIdentityStoreService.hpp"

#include "node/Node.hpp"

//QSharedPointer<LocalIdentityStore> localIdentityStore

LocalIdentityStoreService::LocalIdentityStoreService(QSharedPointer<LocalIdentityStore> localIdentityStore, QStringList dependencies)
	: ServiceWrapper<LocalIdentityStore>(localIdentityStore, "LocalIdentityStore", dependencies)
	, mConfigureHelper("LocalIdentityStoreService", true, true, false, true, false)
{
	OC_METHODGATE();
}


void LocalIdentityStoreService::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
	}
}


void LocalIdentityStoreService::serviceWrapperActivate(QSharedPointer<LocalIdentityStore> localIdentityStore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		localIdentityStore->activate(on);
		if(on) {
			localIdentityStore->synchronize([this, localIdentityStore, callBack](QSharedPointer<SimpleDataStore> sms, bool ok) {
				if(!sms.isNull()) {
					auto map=sms->toMap();
					//qDebug()<<"Local identity synchronized with ok="<<ok<<" and map="<<map;
					//QSharedPointer<Node> mNode;
					if(!mNode.isNull()) {
						mNode->setNodeIdentity(map);
					} else {
						qWarning()<<"ERROR: No node";
					}
					if(nullptr!=callBack) {
						callBack(ok);
					}
				} else {
					qWarning()<<"ERROR: local identity sync sms not ok";
				}
			});
		}
	}
}
