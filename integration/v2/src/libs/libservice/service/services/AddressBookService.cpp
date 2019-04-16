#include "AddressBookService.hpp"



AddressBookService::AddressBookService(QSharedPointer<AddressBook> addressBook, QStringList dependencies)
	: ServiceWrapper<AddressBook>(addressBook, "AddressBook", dependencies)
{
	OC_METHODGATE();
}

void AddressBookService::serviceWrapperActivate(QSharedPointer<AddressBook> addressBook, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	addressBook->activate(on);
	callBack(true);
}
