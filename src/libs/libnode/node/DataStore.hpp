#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "basic/AtomicBoolean.hpp"
#include "utility/Standard.hpp"

#include <QObject>
#include <QSharedPointer>

#include <QRunnable>
#include <QThread>

#include <QDebug>



#include <QDebug>
#include <QThread>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <QWaitCondition>


/*

  DataStore is used to persist data in form of variant maps.

  The long term goal of the API is to ensure compliance with ACID requirements as far as
  possible while also being asynchronous.

  The API should also be as transparent as possible, meaning that using it should
  require the least possible amount of boilerplate.

  ACID is an acronym for the following 4 requirements:

  + Atomicity    - The operation is indivisible, all-or-nothing. A transaction can be "committed" or "rolled back"; if it is committed all of its effects must complete fully, otherwise if it is rolled back it must have no effect.
  + Consistency  - The transaction transforms the database from one consistent state to another consistent state. Here "consistent" implies obeying not just relational integrity constraints but also business rules and semantics, such as that a given Customer record's Address and ZIP fields must not specify inconsistent information (e.g., an address in New York and a ZIP code in Alabama).
  + Isolation    - Two transactions are fully isolated if they can be started simultaneously and the result is guaranteed to be the same as though one was run entirely before the other ("Serializable"). In practice, databases support various levels of isolation, each of which trades off better isolation at the expense of concurrency or vice versa. Some typical isolation levels, in order from best to worst concurrency, are: Read Uncommitted (a.k.a. Dirty Read), Read Committed, Repeatable Read, and Serializable (highest isolation), and describe what kinds of intermediate states of other transactions might be visible.
  + Durability   - If a transaction is committed, its effects are not lost even if there is a subsequent system crash. This requirement is usually implemented using a transaction log which is used to "roll forward" the state of the database on system restart.

*/


enum TransactionType {
	TRANSACTION_CLEAR, TRANSACTION_GET, TRANSACTION_SET, TRANSACTION_LOAD, TRANSACTION_SAVE, TRANSACTION_SYNCHRONIZE, TRANSACTION_DONE
};


class StorageEvent;
class DataStore;



QDebug operator<< (QDebug d, TransactionType tt);


/**
  Private class for Transaction handle
  ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
*/



class StorageEventPrivate
{
private:
	DataStore & mStore;
	const TransactionType mType;
	QVariantMap mData;
	QMutex mStartedMutex;
	QMutex mFinishedMutex;
	QWaitCondition mFinishedCond;
	QMutex mSuccessfullMutex;
	QMutex mMessageMutex;
	bool mStarted;
	bool mFinished;
	bool mSuccessfull;
	QString mMessage;


public:
	explicit StorageEventPrivate(DataStore & store, const TransactionType type, QVariantMap data=QVariantMap());
	virtual ~StorageEventPrivate() {}

public:

	friend class StorageEvent;
};


////////////////////////////////////////////////////////////////////////////////


class StorageEvent
{
private:
	QSharedPointer<StorageEventPrivate>  p_ptr;

public:
	inline StorageEvent() Q_DECL_NOTHROW{}
	virtual ~StorageEvent() {}
public:
	explicit StorageEvent(DataStore & store, const TransactionType type, QVariantMap data=QVariantMap());
	explicit StorageEvent(StorageEventPrivate &pp);

	StorageEvent(const StorageEvent & other);
	StorageEvent(StorageEvent && other);

	StorageEvent & operator=(StorageEvent other);
	bool operator==(StorageEvent &other);
	bool operator!=(StorageEvent &other);

	//friend void swap(Transaction& first, Transaction& second) Q_DECL_NOTHROW;


public:
	DataStore & store() const ;
	TransactionType type() const ;
	QVariantMap data() const ;
	bool isStarted();

	bool isFinished();
	void notifyFinished();
	void waitForFinished();
	template <typename F>
	void onFinished(F callBack);

	bool isSuccessfull();
	QString message();
	bool run();


};



template <typename F>
void StorageEvent::onFinished(F callBack)
{
	OC_METHODGATE()
	waitForFinished();
	callBack(*this);
}


////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ConcurrentQueue
{
private:
	const int mCapacity;

	// Concurrency primiteives to synchronize on queue
	QMutex mMutex;
	QWaitCondition mNotEmpty;
	QWaitCondition mNotFull;

	QLinkedList<T> mItems;

	AtomicBoolean mDone;

public:
	explicit ConcurrentQueue(int capacity=0);
	virtual ~ConcurrentQueue();

public:
	void put(T item);
	T get();
	int count();
	bool isEmpty();
};


template <typename T>
ConcurrentQueue<T>::ConcurrentQueue(int capacity)
	: mCapacity(capacity)
	, mDone(false)
{
	OC_METHODGATE();
}
template <typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
	OC_METHODGATE();
	mDone=true;
	mNotFull.wakeAll();
	mNotEmpty.wakeAll();
}

template <typename T>
void ConcurrentQueue<T>::put(T item)
{
	OC_METHODGATE();
	{
		QMutexLocker ml(&mMutex);
		int count=mItems.count();
		if ((mCapacity > 0) && (count >= mCapacity)) {
			//qDebug()<<"Waiting for not-full with count="<<count;
			mNotFull.wait(&mMutex);
			count=mItems.count();
			//qDebug()<<"Got for not-full with count="<<count;
		}
		mItems.push_front(item);
		//qDebug()<<" + DataStore::enqueueTransaction() appended transaction giving total of "<<mTransactionLog.count();
		//qDebug()<<"Signaling not-empty";
		mNotEmpty.wakeAll();
		//qDebug()<<"'PUT' DONE";
	}
}

template <typename T>
T ConcurrentQueue<T>::get()
{
	OC_METHODGATE();
	// Block until queue actually contains something before fetching the first item
	unsigned long rtto=1000;
	T item;
	QMutexLocker ml(&mMutex);
	int count=mItems.count();
	//qDebug()<<"Waiting for not-empty with count="<<count;
	while((count<=0) && (!mDone)) {
		const bool wok=mNotEmpty.wait(&mMutex, rtto);
		if(!wok){
			qDebug()<<" ... done="<<mDone <<" count="<<count;
		}
		count=mItems.count();
	}
	//qDebug()<<"Got not-empty with count="<<count;
	const bool wasFull=(count >= mCapacity);
	item=mItems.takeLast();
	count=mItems.count();
	const bool isFull=(count < mCapacity);
	//qDebug()<<"State mCapacity="<<mCapacity<<", wasFull="<<wasFull<<", isFull="<<isFull<<", count="<<count;
	if((mCapacity>0) && (wasFull) && (!isFull)) {
		//qDebug()<<"Signaling not-full";
		ml.unlock();
		mNotFull.wakeAll();
		ml.relock();
	}
	//qDebug()<<"'GET' DONE";
	return item;
}


template <typename T>
int ConcurrentQueue<T>::count()
{
	OC_METHODGATE();
	QMutexLocker ml(&mMutex);
	const int count=mItems.count();
	return count;
}


template <typename T>
bool ConcurrentQueue<T>::isEmpty()
{
	OC_METHODGATE();
	return count()<=0;
}


/*
		{
			QMutexLocker ml(&mTransactionLogMutex);
			const int count=mTransactionLog.count();
			if (count >0) {
				trans=mTransactionLog.takeLast();
				gotOne=true;
				//qDebug()<<" + DataStore::processTransactions() fetched transaction leaving behind "<<mTransactionLog.count();
				mTransactionLogNotFull.wakeAll();
			}
		}
		if(gotOne) {
			qDebug()<<" + DataStore::processTransactions() running transaction with type="<<trans.type()<<"	from thread "<<handleCounterString(QThread::currentThreadId());
			trans.run();
			qDebug()<<" + DataStore::processTransactions() done running transaction with type="<<trans.type()<<" from thread "<<handleCounterString(QThread::currentThreadId());
		} else {
			qDebug()<<" + DataStore::processTransactions() no transaction fetched, looping from thread "<<handleCounterString(QThread::currentThreadId());
		}
		return trans;

*/

////////////////////////////////////////////////////////////////////////////////


class DataStore: public QObject
{
	Q_OBJECT
protected:
	QVariantMap mData;
	QString mFilename;

	ConcurrentQueue<StorageEvent> mTransactions;

	QAtomicInteger<quint64> mAutoIncrement;
	QAtomicInteger<quint64> mDiskCounter;
	QAtomicInteger<quint64> mMemoryCounter;
	AtomicBoolean mDone;

	QFuture<void> mCompleteFuture;


public:
	explicit DataStore(QString filename="", QObject *parent = nullptr);
	virtual ~DataStore();


public:

	QString filename() const;
	bool fileExists() const;
	bool ready();


private:

	StorageEvent enqueueTransaction(StorageEvent trans);
	void processTransactions();

	quint64 autoIncrement();

public:
	StorageEvent clear();
	StorageEvent get();
	StorageEvent set(QVariantMap data);
	StorageEvent load();
	StorageEvent save();
	StorageEvent synchronize();



private:

	StorageEvent complete();


private:

	// Delete data on disk and in memory
	bool clearSync();
	// Get data from memory
	QVariantMap getSync();
	// Set data in memory
	bool setSync(QVariantMap data);
	// Load data from file on disk into memory
	bool loadSync();
	// Save data from memory into file on disk
	bool saveSync();

	// Save or load data depending on wether memory or disk is newest
	bool synchronizeSync();

	// Internal transaction to stop transaction processing thread
	bool completeSync();


public:

	friend class StorageEvent;
	friend class StorageEventPrivate;
	friend class SimpleDataStore;
	friend const QDebug &operator<<(QDebug &d, DataStore &ks);
};


const QDebug &operator<<(QDebug &d, DataStore &ks);


////////////////////////////////////////////////////////////////////////////////




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

template <typename F>
void SimpleDataStore::clear(F callBack)
{
	OC_METHODGATE();
	mDataStore.clear().onFinished([=](StorageEvent clear_t) {
		fromMap(QVariantMap());
		const bool clear_ok=clear_t.isSuccessfull();
		if(clear_ok) {
			qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" clear SUCCEEDED";
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
			qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" save SUCCEEDED";
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
		qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load finished with ok="<<load_ok;
		if(load_ok) {
			mDataStore.get().onFinished([=](StorageEvent get_t) {
				const bool get_ok=get_t.isSuccessfull();
				if(get_ok) {
					auto data=get_t.data();
					qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" load-get SUCCEEDED with data="<<data;
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
		qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync finished with ok="<<sync_ok;
		if(sync_ok) {
			mDataStore.get().onFinished([=](StorageEvent get_t) {
				const bool get_ok=get_t.isSuccessfull();
				if(get_ok) {
					auto data=get_t.data();
					qDebug()<<"datastore="<<mDataStore.filename()<<", exists="<<mDataStore.fileExists()<<" sync-get SUCCEEDED with data="<<data;
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




////////////////////////////////////////////////////////////////////////////////


class SimpleMapStore: public SimpleDataStore
{

private:
	QVariantMap mMap;
public:
	explicit SimpleMapStore(QString filename="");
	virtual ~SimpleMapStore();

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data)  Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;
};


#endif // DATASTORE_HPP
