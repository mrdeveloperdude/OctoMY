#ifndef ADDRESSBOOKSERVICE_HPP
#define ADDRESSBOOKSERVICE_HPP


#include "discovery/AddressBook.hpp"
#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

/**
 * @brief The AddressBookService class is what it says on the tin; basically a wrapper to make AddressBook appear as a service.
 */

class AddressBookService: public ServiceWrapper<AddressBook>
{
public:
	AddressBookService(QSharedPointer<AddressBook> addressBook, QStringList dependencies= {});

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<AddressBook> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};

#endif
// ADDRESSBOOKSERVICE_HPP
