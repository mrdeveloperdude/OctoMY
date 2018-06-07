#include "SimpleMapStore.hpp"








SimpleMapStore::SimpleMapStore(QString filename)
	: SimpleDataStore(filename)
{
	OC_METHODGATE();
}

SimpleMapStore::~SimpleMapStore()
{
	OC_METHODGATE();
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
