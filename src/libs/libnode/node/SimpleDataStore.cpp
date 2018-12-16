#include "SimpleDataStore.hpp"





SimpleDataStoreFrontend::SimpleDataStoreFrontend(SimpleDataStore &dataStore)
	: mDataStore(dataStore)
{

}


SimpleDataStoreFrontend::~SimpleDataStoreFrontend()
{

}


////////////////////////////////////////////////////////////////////////////////


SimpleDataStore::SimpleDataStore(QString filename)
	: mJsonBackend(filename)

	, mSimpleFrontend(nullptr)
	, mStore(nullptr)
//	, mSimpleFrontend(OC_NEW SimpleDataStoreFrontend(*this))
//	, mStore(OC_NEW AsyncStore<QVariantMap>(mJsonBackend, *mSimpleFrontend))

{
	OC_METHODGATE();
}


SimpleDataStore::~SimpleDataStore()
{
	OC_METHODGATE();
}



bool SimpleDataStore::fromDefault()
{
	qWarning()<<"PURE VIRTUAL FROM DEF CALLED ON SimpleDataStore";
	return false;
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




bool SimpleDataStoreFrontend::setFrontend(QVariantMap map)
{
	OC_METHODGATE();
	return mDataStore.fromMap(map);
}

QVariantMap SimpleDataStoreFrontend::getFrontend(bool &ok)
{
	OC_METHODGATE();
	ok=true;
	return mDataStore.toMap();
}



bool SimpleDataStoreFrontend::generateFrontend()
{
	OC_METHODGATE();
	bool def=mDataStore.fromDefault();
	qDebug()<<"Default generated was "<<def;
	return def;
}


bool SimpleDataStoreFrontend::clearFrontend()
{
	OC_METHODGATE();
	return mDataStore.fromMap(QVariantMap());
}


bool SimpleDataStore::ensureStore() const
{
	OC_METHODGATE();
	const bool ok=(nullptr!=mStore);
	if(!ok) {
		qWarning()<<"WARNING: Store not present";
	}
	return ok;
}


QString SimpleDataStore::filename() const
{
	OC_METHODGATE();
	if(ensureStore()) {
		return mStore->filename();
	} else {
		return "";
	}
}

bool SimpleDataStore::fileExists()  const
{
	OC_METHODGATE();
	if(ensureStore()) {
		return mStore->fileExists();
	} else {
		return false;
	}
}

bool SimpleDataStore::ready()
{
	OC_METHODGATE();
	if(ensureStore()) {
		return mStore->ready();
	} else {
		return false;
	}
}



#ifdef USE_OLD_STORE

////////////////////////////////////////////////////////////////////////////////





SimpleDataStore::SimpleDataStore(QString filename)
	: mDataStore(filename)
	, mLog(false)
{
	OC_METHODGATE();
	if(mLog) {
		qDebug()<<"SimpleDataStore created for '"<<filename<<"'";
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


#endif
