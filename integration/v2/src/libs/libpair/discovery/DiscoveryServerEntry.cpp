#include "DiscoveryServerEntry.hpp"

#include "uptime/MethodGate.hpp"

#include "address/Associate.hpp"
#include "security/Key.hpp"

DiscoveryServerEntry::DiscoveryServerEntry()
{
	OC_METHODGATE();
}


DiscoveryServerEntry::DiscoveryServerEntry(QSharedPointer<Associate> associate, QSharedPointer<Key> key)
	: mAssociate(associate)
	, mKey(key)
{
	OC_METHODGATE();
	Q_ASSERT(!associate.isNull());
	Q_ASSERT(!key.isNull());
	Q_ASSERT(associate->id()==key->id());
}


DiscoveryServerEntry::~DiscoveryServerEntry()
{
	OC_METHODGATE();
}


bool DiscoveryServerEntry::isNull()
{
	OC_METHODGATE();
	return (mAssociate.isNull() || mKey.isNull());
}

QSharedPointer<Associate> DiscoveryServerEntry::associate()
{
	OC_METHODGATE();
	return mAssociate;
}


QSharedPointer<Key> DiscoveryServerEntry::key()
{
	OC_METHODGATE();
	return mKey;
}


QString DiscoveryServerEntry::id()
{
	OC_METHODGATE();
	return mAssociate.isNull()?"":mAssociate->id();
}


QVariantMap DiscoveryServerEntry::toVariantMap()


{
	OC_METHODGATE();
	QVariantMap map;
	map["associate"]=mAssociate->toVariantMap();
	map["key"]=mKey->toVariantMap(true);
	return map;
}


quint64 DiscoveryServerEntry::lastSeen()
{
	OC_METHODGATE();
	return mAssociate.isNull()?0:mAssociate->lastSeen();
}


bool DiscoveryServerEntry::isValidForServer()
{
	OC_METHODGATE();
	return mAssociate.isNull()?false:mAssociate->isValidForServer();
}


QString DiscoveryServerEntry::toString()
{
	OC_METHODGATE();
	return mAssociate.isNull()?"{null}":mAssociate->toString();
}


QStringList DiscoveryServerEntry::pins()
{
	OC_METHODGATE();
	return mAssociate.isNull()?QStringList():mAssociate->pins();
}
