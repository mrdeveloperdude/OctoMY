#include "AddressBookService.hpp"

#include "node/Node.hpp"
#include "client/ClientList.hpp"


AddressBookService::AddressBookService(QSharedPointer<AddressBook> addressBook, QStringList dependencies)
	: ServiceWrapper<AddressBook>(addressBook, "AddressBook", dependencies)
	, mConfigureHelper("AddressBookService", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
{
	OC_METHODGATE();
}


void AddressBookService::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
	}
}


void AddressBookService::serviceWrapperActivate(QSharedPointer<AddressBook> addressBook, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		addressBook->activate(on, [this, addressBook, callBack, on](bool ok) {
			if(!addressBook.isNull()) {
				//qDebug()<<"AddressBook synchronized with ok="<<ok;
				auto map=addressBook->toMap();
				//mNode->cl
				auto clientList=mNode->clientList();
				if(!clientList.isNull()) {
					clientList->syncToAddressBook(addressBook, mNode);
				} else {
					qWarning()<<"ERROR: no clientlist";
				}
			} else {
				qWarning()<<"ERROR: address book not ok";
			}
			if(nullptr!=callBack) {
				callBack(on, ok);
			}
		});
	}
}
