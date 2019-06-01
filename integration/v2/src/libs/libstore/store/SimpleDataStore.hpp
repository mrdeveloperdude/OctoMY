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

	/*
	qint64 mLastSync;
	qint64 mMaxSyncInterval;
	QTimer mSyncTimer;
*/

private:
	static const qint64 TIMED_SYNC_OFF;


public:
	explicit SimpleDataStore();
	virtual ~SimpleDataStore();

// ConfigureHelper interface
public:
	void configure(QString filename, qint64 maxSyncInterval=TIMED_SYNC_OFF);
	void activate(const bool on, std::function<void(bool)> callBack=nullptr);

	/*
	// Timed sync interface
private:
	void timerSync();
	void delayedSync();
*/
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
	void synchronize();


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
			/*
			if(ok){
				timerSync();
			}
			*/
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



#endif
// SIMPLEDATASTORE_HPP
