#ifndef DISCOVERYSERVER_HPP
#define DISCOVERYSERVER_HPP

#include "address/Associate.hpp"
#include "DiscoveryServerSession.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include "DiscoveryServerEntry.hpp"

#include <QMap>



/**
 * DiscoveryServer is part fo the discovery process. Please see full description
 * in DiscoveryClient.hpp
 */

class DiscoveryServer
{
private:
	// The active sessions
	QMap<QString, DiscoveryServerSession *> mRegistry;
	// Flag to determine if we are logging or not
	bool mLog;

public:
	explicit DiscoveryServer(bool log = false);
	virtual ~DiscoveryServer();

public:
	// Handle incomming request
	DiscoveryServerSession *request(DiscoveryServerEntry part);
	// Remove inactive sessions
	void prune(quint64 deadline);
	// Export full state to map
	QVariantList toVariantList();

};
#endif
// DISCOVERYSERVER_HPP
