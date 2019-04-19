#ifndef SIMPLEDATASTORE_HPP
#define SIMPLEDATASTORE_HPP

#include "AsyncStore.hpp"

#include "JsonAsyncBackend.hpp"
#include "uptime/ConfigureHelper.hpp"


#include <QDebug>
#include <QVariantMap>
#include <QString>
#include <QSharedPointer>
#include <QEnableSharedFromThis>

class SimpleDataStore;

class SimpleDataStoreFrontend: public AsyncFrontend<QVariantMap>
{

private:
	QSharedPointer<SimpleDataStore> mDataStore;

public:
	SimpleDataStoreFrontend(QSharedPointer<SimpleDataStore> dataStore);
	virtual ~SimpleDataStoreFrontend();

	// AsyncFrontend interface
public:
	virtual bool clearFrontend() Q_DECL_OVERRIDE;
	virtual bool setFrontend(QVariantMap data) Q_DECL_OVERRIDE;
	virtual QVariantMap getFrontend(bool &ok) Q_DECL_OVERRIDE;
	virtual bool generateFrontend() Q_DECL_OVERRIDE;

};


////////////////////////////////////////////////////////////////////////////////


class SimpleDataStore: public QEnableSharedFromThis<SimpleDataStore>
{

private:
	QSharedPointer<JsonAsyncBackend> mJsonBackend;
	QSharedPointer<SimpleDataStoreFrontend > mSimpleFrontend;
	QSharedPointer<AsyncStore<QVariantMap> > mStore;
	ConfigureHelper mConfigureHelper;

public:
	explicit SimpleDataStore();
	virtual ~SimpleDataStore();

// ConfigureHelper interface
public:
	void configure(QString filename);
	void activate(const bool on, std::function<void(bool)> callBack=nullptr);

	// SimpleDataStore interface
public:
	// NOTE: Must accept empty map, beacuse calling fromMap with empty map is the semantic for clearing
	virtual bool fromMap(QVariantMap data) =0;
	virtual QVariantMap toMap() =0;
	// This is the "bootstrapping alibi", that is this is called when there is no data on disk at load time to initialize the first time.
	virtual bool fromDefault();


public:

	bool ensureStore() const;

	QString filename()  const;
	bool fileExists()  const;
	bool ready();


	template <typename F>
	void status(F callBack);
	template <typename F>
	void clear(F callBack);
	void clear();
	template <typename F>
	void save(F callBack);
	void save();
	template <typename F>
	void load(F callBack);
	void load();
	template <typename F>
	void synchronize(F callBack);


};



////////////////////////////////////////////////////////////////////////////////

template <typename F>
void SimpleDataStore::status(F callBack)
{
	OC_METHODGATE();
	if(ensureStore()) {
		mStore->status().onFinished(
		[this, callBack](ASEvent<QVariantMap> &ase) {
			const bool ok=ase.isSuccessfull();
			callBack(sharedFromThis(), ok);
		}
		);
	}
}

template <typename F>
void SimpleDataStore::clear(F callBack)
{
	OC_METHODGATE();
	if(ensureStore()) {
		mStore->clear().onFinished(
		[this, callBack](ASEvent<QVariantMap> &ase) {
			const bool ok=ase.isSuccessfull();
			callBack(sharedFromThis(), ok);
		}
		);
	}
}

template <typename F>
void SimpleDataStore::save(F callBack)
{
	OC_METHODGATE();
	if(ensureStore()) {
		mStore->save().onFinished(
		[this, callBack](ASEvent<QVariantMap> &ase) {
			const bool ok=ase.isSuccessfull();
			callBack(sharedFromThis(), ok);
		}
		);
	}
}

template <typename F>
void SimpleDataStore::load(F callBack)
{
	OC_METHODGATE();
	if(ensureStore()) {
		mStore->load().onFinished(
		[this, callBack](ASEvent<QVariantMap> &ase) {
			const bool ok=ase.isSuccessfull();
			callBack(sharedFromThis(), ok);
		}
		);
	}
}

template <typename F>
void SimpleDataStore::synchronize(F callBack)
{
	OC_METHODGATE();
	if(ensureStore()) {
		mStore->synchronize().onFinished(
		[this, callBack](ASEvent<QVariantMap> &ase) {
			const bool ok=ase.isSuccessfull();
			callBack(sharedFromThis(), ok);
		}
		);
	}
}


#ifdef USE_OLD_STORE

#include "node/DataStore.hpp"


// Convenience wrapper for DataStore
class SimpleDataStore
{
private:
	DataStore mDataStore;
	bool mLog;

public:

	explicit SimpleDataStore(QString filename="");
	virtual ~SimpleDataStore();

	// SimpleDataStore interface
public:
	// NOTE: Must accept empty map, beacuse calling fromMap with empty map is the semantic for clearing
	virtual bool fromMap(QVariantMap data)=0;
	virtual QVariantMap toMap() =0;
	// This is the "bootstrapping alibi", that is this is called when there is no data on disk at load time to initialize the first time.
	virtual bool fromDefault()=0;

public:

	QString filename() const;
	bool fileExists() const;
	bool ready();

	template <typename F>
	void clear(F callBack);
	template <typename F>
	void save(F callBack);
	template <typename F>
	void load(F callBack);
	template <typename F>
	void synchronize(F callBack);

	void clear();
	void save();
	void load();
	void synchronize();
};

////////////////////////////////////////////////////////////////////////////////

template <typename F>
void SimpleDataStore::clear(F callBack)
{
	OC_METHODGATE();
	mDataStore.clear().onFinished([=](StorageEvent clear_t) {
		// Calling fromMap() with empty map signals "clear"
		fromMap(QVariantMap());
		const bool clear_ok=clear_t.isSuccessfull();
		if(clear_ok) {
			if(mLog) {
				qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" clear SUCCEEDED";
			}
		} else {
			qWarning()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" clear FAILED";
		}
		callBack(*this, clear_ok);
	});
}

template <typename F>
void SimpleDataStore::save(F callBack)
{
	OC_METHODGATE();
	mDataStore.set(toMap());
	mDataStore.save().onFinished([=](StorageEvent save_t) {
		const bool save_ok=save_t.isSuccessfull();
		if(save_ok) {
			if(mLog) {
				qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" save SUCCEEDED";
			}
		} else {
			qWarning()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" save FAILED";
		}
		callBack(*this, save_ok);
	});
}



template <typename F>
void SimpleDataStore::load(F callBack)
{
	OC_METHODGATE();
	mDataStore.load().onFinished([=](StorageEvent load_t) {
		const bool load_ok=load_t.isSuccessfull();
		if(mLog) {
			qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load finished with ok="<<load_ok;
		}
		if(load_ok) {
			mDataStore.get().onFinished([=](StorageEvent get_t) {
				const bool get_ok=get_t.isSuccessfull();
				if(get_ok) {
					auto data=get_t.data();
					if(mLog) {
						qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load-get SUCCEEDED with data="<<data;
					}
					fromMap(data);
				} else {
					qWarning()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load-get FAILED";
				}
				callBack(*this, get_ok);
			});
		} else {
			qWarning()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load FAILED";
			callBack(*this, false);
		}
	});
}



template <typename F>
void SimpleDataStore::synchronize(F callBack)
{
	OC_METHODGATE();
	mDataStore.synchronize().onFinished([=](StorageEvent sync_t) {
		const bool sync_ok=sync_t.isSuccessfull();
		if(mLog) {
			qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync finished with ok="<<sync_ok;
		}
		if(sync_ok) {
			mDataStore.get().onFinished([=](StorageEvent get_t) {
				const bool get_ok=get_t.isSuccessfull();
				if(get_ok) {
					auto data=get_t.data();
					if(mLog) {
						qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync-get SUCCEEDED with data="<<data;
					}
					fromMap(data);
				} else {
					qWarning()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync-get FAILED";
				}
				callBack(*this, get_ok);
			});
		} else {
			qWarning()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync FAILED";
			callBack(*this, false);
		}
	});
}

#endif

#endif // SIMPLEDATASTORE_HPP
