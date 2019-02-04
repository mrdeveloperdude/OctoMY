#include "SimpleMapStore.hpp"

#include "uptime/MethodGate.hpp"

#include <QVariantMap>


SimpleMapStore::SimpleMapStore()
{
	OC_METHODGATE();
}

SimpleMapStore::~SimpleMapStore()
{
	OC_METHODGATE();
}

void SimpleMapStore::configure(QString filename)
{
	OC_METHODGATE();
	SimpleDataStore::configure(filename);
}


bool SimpleMapStore::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	mMap=data;
	return true;
}

QVariantMap SimpleMapStore::toMap()
{
	OC_METHODGATE();
	return mMap;
}

bool SimpleMapStore::fromDefault()
{
	OC_METHODGATE();
	mMap=QVariantMap();
	return true;
}
