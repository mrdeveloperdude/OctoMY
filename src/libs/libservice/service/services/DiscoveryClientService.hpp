#ifndef DISCOVERYCLIENTSERVICE_HPP
#define DISCOVERYCLIENTSERVICE_HPP


#include "discovery/DiscoveryClient.hpp"
#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include "uptime/SharedPointerWrapper.hpp"

class Node;

/**
 * @brief The DiscoveryClientService class is what it says on the tin; basically
 * a wrapper to make DiscoveryClient appear as a service.
 */

class DiscoveryClientService: public ServiceWrapper<DiscoveryClient>
{
private:
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	DiscoveryClientService(QSharedPointer<DiscoveryClient> discoveryClient, QStringList dependencies= {});

public:
	void configure(QSharedPointer<Node> node);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<DiscoveryClient> wrapee, bool on, ServiceActivatedCallback callBack) override;

};


#endif
// DISCOVERYCLIENTSERVICE_HPP
