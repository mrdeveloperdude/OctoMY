#ifndef SIMPLEDATASTORE_HPP
#define SIMPLEDATASTORE_HPP

#include "node/DataStore.hpp"


// Convenience wrapper for DataStore
class SimpleDataStore
{
private:
	DataStore mDataStore;

public:

	explicit SimpleDataStore(QString filename="");
	virtual ~SimpleDataStore();

	// SimpleDataStore interface
public:
	// NOTE: Must accept empty map, beacuse calling fromMap with empty map is the semantic for clearing
	virtual bool fromMap(QVariantMap data)=0;
	virtual QVariantMap toMap() =0;

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
			//qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" clear SUCCEEDED";
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
			//qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" save SUCCEEDED";
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
		//qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load finished with ok="<<load_ok;
		if(load_ok) {
			mDataStore.get().onFinished([=](StorageEvent get_t) {
				const bool get_ok=get_t.isSuccessfull();
				if(get_ok) {
					auto data=get_t.data();
					//qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load-get SUCCEEDED with data="<<data;
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
		//qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync finished with ok="<<sync_ok;
		if(sync_ok) {
			mDataStore.get().onFinished([=](StorageEvent get_t) {
				const bool get_ok=get_t.isSuccessfull();
				if(get_ok) {
					auto data=get_t.data();
					//qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync-get SUCCEEDED with data="<<data;
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







#endif // SIMPLEDATASTORE_HPP
