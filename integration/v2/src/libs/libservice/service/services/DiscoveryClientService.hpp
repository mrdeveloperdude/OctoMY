#ifndef DISCOVERYCLIENTSERVICE_HPP
#define DISCOVERYCLIENTSERVICE_HPP


#include "discovery/DiscoveryClient.hpp"
#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

/**
 * @brief The DiscoveryClientService class is what it says on the tin; basically a wrapper to make DiscoveryClient appear as a service.
 */

class DiscoveryClientService: public ServiceWrapper<DiscoveryClient>
{
public:
	DiscoveryClientService(QSharedPointer<DiscoveryClient> discoveryClient, QStringList dependencies= {});

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<DiscoveryClient> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};



#endif // DISCOVERYCLIENTSERVICE_HPP
