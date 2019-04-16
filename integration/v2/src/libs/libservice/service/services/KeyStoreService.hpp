#ifndef KEYSTORESERVICE_HPP
#define KEYSTORESERVICE_HPP

#include "security/KeyStore.hpp"
#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

/**
 * @brief The KeyStoreService class is what it says on the tin; basically a wrapper to make KeyStore appear as a service.
 * Basically KeyStore is not really well suited to be a service since it will never "unload", but that is ok as having it
 * loaded really does not cost us anything. But it is important to have it work as a service since other services will
 * depend on it during loading.
 */

class KeyStoreService: public ServiceWrapper<KeyStore>
{
public:
	KeyStoreService(QSharedPointer<KeyStore> keystore);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<KeyStore> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};


#endif
// KEYSTORESERVICE_HPP
