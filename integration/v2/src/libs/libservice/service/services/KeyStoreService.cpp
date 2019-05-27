#include "KeyStoreService.hpp"


KeyStoreService::KeyStoreService(QSharedPointer<KeyStore> keystore, QStringList dependencies)
	: ServiceWrapper<KeyStore>(keystore, "KeyStore", dependencies)
	, mConfigureHelper("KeyStoreService", true, true, false, true, false)
{
	OC_METHODGATE();
}


void KeyStoreService::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {

	}
}


void KeyStoreService::serviceWrapperActivate(QSharedPointer<KeyStore> keystore, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(!keystore.isNull()) {
		keystore->activate(on, [callBack, on](bool ok) {
			if(nullptr!=callBack) {
				callBack(on, ok);
			}
		});
	} else {
		qWarning()<<"ERROR: No keystore";
		if(nullptr!=callBack) {
			callBack(on, false);
		}
	}

}
