#ifndef DISCOVERYSERVERENTRY_HPP
#define DISCOVERYSERVERENTRY_HPP


#include "uptime/SharedPointerWrapper.hpp"

class Associate;
class Key;

class DiscoveryServerEntry
{
private:
	QSharedPointer<Associate> mAssociate;
	QSharedPointer<Key> mKey;
public:
	// For containers that need it
	explicit DiscoveryServerEntry();
	explicit DiscoveryServerEntry(QSharedPointer<Associate> associate, QSharedPointer<Key> key);
	virtual ~DiscoveryServerEntry();

public:
	bool isNull();
	QSharedPointer<Associate> associate();
	QSharedPointer<Key> key();

	QString id();
	QVariantMap toVariantMap();
	quint64 lastSeen();
	bool isValidForServer();

	QString toString();

	QStringList pins();

};

#endif
// DISCOVERYSERVERENTRY_HPP
