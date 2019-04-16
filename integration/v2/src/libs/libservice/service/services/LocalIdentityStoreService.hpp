#ifndef LOCALIDENTITYSTORESERVICE_HPP
#define LOCALIDENTITYSTORESERVICE_HPP

#include "node/LocalIdentityStore.hpp"
#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

/**
 * @brief The LocalIdentityStoreService class is what it says on the tin; basically a wrapper to make LocalIdentityStore appear as a service.
 */

class LocalIdentityStoreService: public ServiceWrapper<LocalIdentityStore>
{
public:
	LocalIdentityStoreService(QSharedPointer<LocalIdentityStore> localIdentityStore, QStringList dependencies={"KeyStore"});

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<LocalIdentityStore> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};



#endif
// LOCALIDENTITYSTORESERVICE_HPP
