#include "LocalAddressListService.hpp"

LocalAddressListService::LocalAddressListService(QSharedPointer<LocalAddressList> localAddressList, QStringList dependencies)
	: ServiceWrapper<LocalAddressList>(localAddressList, "LocalAddressList", dependencies)
	, mConfigureHelper("LocalAddressListService", true, true, false, true, false)
{
	OC_METHODGATE();
}


void LocalAddressListService::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {

	}
}


void LocalAddressListService::serviceWrapperActivate(QSharedPointer<LocalAddressList> localAddressList, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	localAddressList->activate(on);
	if(nullptr!= callBack) {
		callBack(on, true);
	}
}
