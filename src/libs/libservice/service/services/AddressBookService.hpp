#ifndef ADDRESSBOOKSERVICE_HPP
#define ADDRESSBOOKSERVICE_HPP


#include "discovery/AddressBook.hpp"
#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include "uptime/SharedPointerWrapper.hpp"

/**
 * @brief The AddressBookService class is what it says on the tin; basically a wrapper to make AddressBook appear as a service.
 */

class AddressBookService: public ServiceWrapper<AddressBook>
{
private:
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	AddressBookService(QSharedPointer<AddressBook> addressBook, QStringList dependencies= {});

public:
	void configure(QSharedPointer<Node> node);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<AddressBook> wrapee, bool on, ServiceActivatedCallback callBack) override;

};

#endif
// ADDRESSBOOKSERVICE_HPP
