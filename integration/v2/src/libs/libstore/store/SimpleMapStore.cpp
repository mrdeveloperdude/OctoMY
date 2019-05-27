#include "SimpleMapStore.hpp"

#include "uptime/MethodGate.hpp"

#include <QVariantMap>


SimpleMapStore::SimpleMapStore()
	: mConfigureHelper("SimpleMapStore", true, true, false, true, false)
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
	if(mConfigureHelper.configure()) {
		SimpleDataStore::configure(filename);
	}
}


void SimpleMapStore::activate(const bool on, std::function<void(bool)> callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		SimpleDataStore::activate(on, callBack);
	}
}


bool SimpleMapStore::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mMap=data;
		return true;
	}
	return false;
}


QVariantMap SimpleMapStore::toMap()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mMap;
	}
	return QVariantMap();
}


bool SimpleMapStore::fromDefault()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mMap=QVariantMap();
		return true;
	}
	return false;
}
