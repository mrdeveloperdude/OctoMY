#include "SimpleDataStore.hpp"



////////////////////////////////////////////////////////////////////////////////




SimpleDataStore::SimpleDataStore(QString filename)
	: mDataStore(filename)
	, mLog(true)
{
	OC_METHODGATE();
	if(mLog){
		qDebug()<<"SimpleDataStore created with logging enabled";
	}
}

SimpleDataStore::~SimpleDataStore()
{
	OC_METHODGATE();
}



QString SimpleDataStore::filename() const
{
	OC_METHODGATE();
	return mDataStore.filename();
}

bool SimpleDataStore::fileExists() const
{
	OC_METHODGATE();
	return mDataStore.fileExists();
}

bool SimpleDataStore::ready()
{
	OC_METHODGATE();
	return mDataStore.ready();
}


void SimpleDataStore::clear()
{
	OC_METHODGATE();
	clear([](SimpleDataStore &, bool) {});
}
void SimpleDataStore::save()
{
	OC_METHODGATE();
	save([](SimpleDataStore &, bool) {});
}
void SimpleDataStore::load()
{
	OC_METHODGATE();
	load([](SimpleDataStore &, bool) {});
}
void SimpleDataStore::synchronize()
{
	OC_METHODGATE();
	synchronize([](SimpleDataStore &, bool) {});
}


