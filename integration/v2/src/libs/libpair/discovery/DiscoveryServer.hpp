#ifndef DISCOVERYSERVER_HPP
#define DISCOVERYSERVER_HPP

#include "address/Associate.hpp"
#include "DiscoveryServerSession.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QMap>

/**
 * DiscoveryServer is part fo the discovery process. Please see full description
 * in DiscoveryClient.hpp
 */

class DiscoveryServer
{
private:
	QMap<QString, DiscoveryServerSession *> mRegistry;
	bool mLog;

public:
	explicit DiscoveryServer(bool log = false);
	virtual ~DiscoveryServer();

public:
	DiscoveryServerSession *request(QSharedPointer<Associate> part);
	void prune(quint64 deadline);
	QVariantList toVariantList();

};
#endif
// DISCOVERYSERVER_HPP
