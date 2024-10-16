#ifndef DISCOVERYSERVER_HPP
#define DISCOVERYSERVER_HPP

#include "basic/Associate.hpp"
#include "DiscoveryServerSession.hpp"

#include <QSharedPointer>

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
#endif // DISCOVERYSERVER_HPP
