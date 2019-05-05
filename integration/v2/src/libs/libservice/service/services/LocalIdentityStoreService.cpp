#include "LocalIdentityStoreService.hpp"

#include "node/Node.hpp"

//QSharedPointer<LocalIdentityStore> localIdentityStore

LocalIdentityStoreService::LocalIdentityStoreService(QSharedPointer<Node> node, QStringList dependencies)
	: ServiceWrapper<LocalIdentityStore>(node->localIdentityStore(), "LocalIdentityStore", dependencies)
	, mNode(node)
{
	OC_METHODGATE();
}


void LocalIdentityStoreService::serviceWrapperActivate(QSharedPointer<LocalIdentityStore> localIdentityStore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	localIdentityStore->activate(on);
	if(on) {
		localIdentityStore->synchronize([this, localIdentityStore, callBack](QSharedPointer<SimpleDataStore> sms, bool ok) {
			if(!sms.isNull()) {
				auto map=sms->toMap();
				//qDebug()<<"Local identity synchronized with ok="<<ok<<" and map="<<map;
				//QSharedPointer<Node> mNode;
				mNode->setNodeIdentity(map);
				if(nullptr!=callBack) {
					callBack(ok);
				}
			} else {
				qWarning()<<"ERROR: local identity sync sms not ok";
			}
		});
	}

}
