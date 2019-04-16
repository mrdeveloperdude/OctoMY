#include "KeyStoreService.hpp"


KeyStoreService::KeyStoreService(QSharedPointer<KeyStore> keystore)
	: ServiceWrapper<KeyStore>(keystore, "KeyStore")
{
	OC_METHODGATE();
}

void KeyStoreService::serviceWrapperActivate(QSharedPointer<KeyStore> keystore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	// NOTE: This will only load and not unload (i.e. the value of the 'on' boolean does not matter).
	//       Unloading is never necessary so this is fine.
	keystore->synchronize([this, on, callBack](ASEvent<QVariantMap> &se) {
		const bool ok=se.isSuccessfull();
		callBack(ok);
	});
}
