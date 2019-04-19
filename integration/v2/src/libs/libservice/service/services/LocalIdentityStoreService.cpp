#include "LocalIdentityStoreService.hpp"


LocalIdentityStoreService::LocalIdentityStoreService(QSharedPointer<LocalIdentityStore> localIdentityStore, QStringList dependencies)
	: ServiceWrapper<LocalIdentityStore>(localIdentityStore, "LocalIdentityStore", dependencies)
{
	OC_METHODGATE();
}

void LocalIdentityStoreService::serviceWrapperActivate(QSharedPointer<LocalIdentityStore> localIdentityStore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	localIdentityStore->activate(on);
	callBack(true);
	localIdentityStore->activate(on);
	/*
	mLocalIdentityStore->synchronize([this, on](QSharedPointer<SimpleDataStore> sms, bool ok) {
		if(!sms.isNull()) {
			auto map=sms->toMap();
			//qDebug()<<"Local identity synchronized with ok="<<ok<<" and map="<<map;
			setNodeIdentity(map);
			mNodeActivationState.localIdentityOK=ok;
			stepActivation(on);
		} else {
			qWarning()<<"ERROR: local identity sync sms not ok";
		}
	});
	*/

}
