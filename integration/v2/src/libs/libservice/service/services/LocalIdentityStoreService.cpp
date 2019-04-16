#include "LocalIdentityStoreService.hpp"


LocalIdentityStoreService::LocalIdentityStoreService(QSharedPointer<LocalIdentityStore> localIdentityStore)
	: ServiceWrapper<LocalIdentityStore>(localIdentityStore, "LocalIdentityStore", QStringList{"KeyStore"})
{
	OC_METHODGATE();
}

void LocalIdentityStoreService::serviceWrapperActivate(QSharedPointer<LocalIdentityStore> localIdentityStore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	localIdentityStore->activate(on);
	callBack(true);
}
