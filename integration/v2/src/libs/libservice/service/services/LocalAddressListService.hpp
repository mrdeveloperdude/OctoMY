#ifndef LOCALADDRESSLISTSERVICE_HPP
#define LOCALADDRESSLISTSERVICE_HPP

#include "address/LocalAddressList.hpp"
#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

/**
 * @brief The LocalAddressListService class is what it says on the tin; basically a wrapper to make LocalAddressList appear as a service.
 */

class LocalAddressListService: public ServiceWrapper<LocalAddressList>
{
public:
	LocalAddressListService(QSharedPointer<LocalAddressList> localAddressList, QStringList dependencies=QStringList());

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<LocalAddressList> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};



#endif
// LOCALADDRESSLISTSERVICE_HPP
