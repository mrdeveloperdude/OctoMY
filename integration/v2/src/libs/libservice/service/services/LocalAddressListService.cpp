#include "LocalAddressListService.hpp"

LocalAddressListService::LocalAddressListService(QSharedPointer<LocalAddressList> localAddressList, QStringList dependencies)
	: ServiceWrapper<LocalAddressList>(localAddressList, "LocalAddressList", dependencies)
{
	OC_METHODGATE();
}

void LocalAddressListService::serviceWrapperActivate(QSharedPointer<LocalAddressList> localAddressList, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	localAddressList->activate(on);
	callBack(true);
}
