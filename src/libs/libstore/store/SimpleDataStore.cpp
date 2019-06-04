#include "SimpleDataStore.hpp"

#include "app/Constants.hpp"

#include "uptime/ConnectionType.hpp"
#include "uptime/New.hpp"

#include "utility/time/HumanTime.hpp"

#include <QTimer>
#include <QObject>

SimpleDataStoreFrontend::SimpleDataStoreFrontend(QSharedPointer<SimpleDataStore> dataStore)
	: mDataStore(dataStore)
{

}


SimpleDataStoreFrontend::~SimpleDataStoreFrontend()
{

}


SimpleDataStore::SimpleDataStore()
	: mConfigureHelper("SimpleDataStore", true, true, true, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	  //, mMaxSyncInterval(TIMED_SYNC_OFF)
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
	auto f=sharedFromThis();
	if(!f.isNull()) {
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
	} else {
		qWarning()<<"ERROR: sharedThis returned null. You should allocate SimpleDataStore as a QSharedPointer for it to work";
	}
}


void SimpleDataStore::activate(const bool on, std::function<void(bool)> callBack)
{
	OC_METHODGATE();
	if(on) {
		if(mConfigureHelper.activate(on)) {
			if(!mStore.isNull()) {
				mStore->activate(on);
				synchronize([callBack](QSharedPointer<SimpleDataStore> s, bool ok) {
					Q_UNUSED(s);
					if(nullptr!=callBack) {
						callBack(ok);
					}
				});
				return;
			}
		}
	} else {
		if(mConfigureHelper.isActivatedAsExpected()) {
			if(!mStore.isNull()) {
				synchronize([this, callBack, on](QSharedPointer<SimpleDataStore> s,bool ok) {
					Q_UNUSED(s);
					if(nullptr!=callBack) {
						callBack(ok);
					}
					mStore->activate(on);
					//mStore->activate(on, callBack);
					mConfigureHelper.activate(on);
				});
			}
			mConfigureHelper.activate(on);
			return;
		}
	}
	if(nullptr!=callBack) {
		callBack(false);
	}
}


bool SimpleDataStore::fromDefault()
{
	OC_METHODGATE();
	qWarning()<<"PURE VIRTUAL fromDefault() CALLED ON SimpleDataStore";
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


void SimpleDataStore::synchronize()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		synchronize([](QSharedPointer<SimpleDataStore>, bool) {});
	}
}


void SimpleDataStore::setSynchronousMode(bool s)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mStore->setSynchronousMode(s);
	}
}


bool SimpleDataStore::ensureStore() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		const bool ok=(!mStore.isNull());
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
	const auto ret=mDataStore->fromMap(QVariantMap());
	return ret;
}

