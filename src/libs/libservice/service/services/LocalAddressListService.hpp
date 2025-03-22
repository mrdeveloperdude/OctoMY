#ifndef LOCALADDRESSLISTSERVICE_HPP
#define LOCALADDRESSLISTSERVICE_HPP

#include "address/LocalAddressList.hpp"
#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include "uptime/SharedPointerWrapper.hpp"

/**
 * @brief The LocalAddressListService class is what it says on the tin; basically a wrapper to make LocalAddressList appear as a service.
 */

class LocalAddressListService: public ServiceWrapper<LocalAddressList>
{
private:
	ConfigureHelper mConfigureHelper;

	public:
	LocalAddressListService(QSharedPointer<LocalAddressList> localAddressList, QStringList dependencies=QStringList());

public:
	void configure();

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<LocalAddressList> wrapee, bool on, ServiceActivatedCallback callBack) override;

};



#endif
// LOCALADDRESSLISTSERVICE_HPP
