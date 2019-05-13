#include "KeyStoreService.hpp"


KeyStoreService::KeyStoreService(QSharedPointer<KeyStore> keystore, QStringList dependencies)
	: ServiceWrapper<KeyStore>(keystore, "KeyStore", dependencies)
{
	OC_METHODGATE();
}

void KeyStoreService::serviceWrapperActivate(QSharedPointer<KeyStore> keystore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	// NOTE: This will only load and not unload (i.e. the value of the 'on' boolean does not matter).
	//       Unloading is never necessary so this is fine.
	if(!keystore.isNull()) {
		keystore->activate(on, [callBack](bool ok) {
			if(nullptr!=callBack) {
				callBack(ok);
			}
		});
	} else {
		qWarning()<<"ERROR: No keystore";
		if(nullptr!=callBack) {
			callBack(false);
		}
	}

}
