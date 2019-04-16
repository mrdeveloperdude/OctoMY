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
}
