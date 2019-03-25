#include "SimpleDataStore.hpp"

#include "app/Constants.hpp"


SimpleDataStoreFrontend::SimpleDataStoreFrontend(QSharedPointer<SimpleDataStore> dataStore)
	: mDataStore(dataStore)
{

}


SimpleDataStoreFrontend::~SimpleDataStoreFrontend()
{

}


////////////////////////////////////////////////////////////////////////////////


SimpleDataStore::SimpleDataStore()
	: mConfigureHelper("SimpleDataStore", true, true, true, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
{
	OC_METHODGATE();
}


SimpleDataStore::~SimpleDataStore()
{
	OC_METHODGATE();
}



void SimpleDataStore::configure(QString filename)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(!mStore.isNull()) {
			mStore=nullptr;
		}
		if(!mSimpleFrontend.isNull()) {
			mSimpleFrontend=nullptr;
		}
		if(!mJsonBackend.isNull()) {
			mJsonBackend=nullptr;
		}
		auto f=sharedFromThis();
		mSimpleFrontend=QSharedPointer<SimpleDataStoreFrontend>(OC_NEW SimpleDataStoreFrontend(f));
		// TODO: Check and prevent double calling this method
		mJsonBackend=QSharedPointer<JsonAsyncBackend>(OC_NEW JsonAsyncBackend());
		if(!mJsonBackend.isNull()) {
			mJsonBackend->configure(filename);
		}
		mStore=QSharedPointer<AsyncStore<QVariantMap> >(OC_NEW AsyncStore<QVariantMap>());
		if(!mStore.isNull()) {
			mStore->configure(mJsonBackend, mSimpleFrontend);
		}
	}
}


void SimpleDataStore::activate(const bool on)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		if(!mStore.isNull()) {
			mStore->activate(on);
		}
	}
}





bool SimpleDataStore::fromDefault()
{
	OC_METHODGATE();
	qWarning()<<"PURE VIRTUAL FROM DEF CALLED ON SimpleDataStore";
	return false;
}


void SimpleDataStore::clear()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		clear([](QSharedPointer<SimpleDataStore>, bool) {});
	}
}

void SimpleDataStore::save()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		save([](QSharedPointer<SimpleDataStore>, bool) {});
	}
}

void SimpleDataStore::load()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		load([](QSharedPointer<SimpleDataStore>, bool) {});
	}
}


bool SimpleDataStore::ensureStore() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		const bool ok=(nullptr!=mStore);
		if(!ok) {
			qWarning()<<"WARNING: Store not present";
		}
		return ok;
	} else {
		return false;
	}
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


////////////////////////////////////////////////////////////////////////////////

bool SimpleDataStoreFrontend::setFrontend(QVariantMap map)
{
	OC_METHODGATE();
	return mDataStore->fromMap(map);

}

QVariantMap SimpleDataStoreFrontend::getFrontend(bool &ok)
{
	OC_METHODGATE();

	ok=true;
	return mDataStore->toMap();

}



bool SimpleDataStoreFrontend::generateFrontend()
{
	OC_METHODGATE();

	bool def=mDataStore->fromDefault();
	qDebug()<<"Default generated was "<<def;
	return def;

}


bool SimpleDataStoreFrontend::clearFrontend()
{
	OC_METHODGATE();
	return mDataStore->fromMap(QVariantMap());
}

