#ifndef ASYNCSTORE_HPP
#define ASYNCSTORE_HPP


#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "utility/ConcurrentQueue.hpp"

#include "node/ASEvent.hpp"




//#include "node/DataStoreInterface.hpp"
#include <QByteArray>
#include <QDebug>
#include <QJsonParseError>
#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QSharedPointer>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
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


////////////////////////////////////////////////////////////////////////////////

template <typename T>
class AsyncBackend
{
	// AsyncBackend interface
public:
	virtual void setFilenameBackend(QString filename) = 0;
	virtual QString filenameBackend() = 0;
	virtual bool existsBackend() = 0;
	virtual bool clearBackend()=0;
	virtual bool saveBackend(T data) = 0;
	virtual T loadBackend(bool &ok) = 0;

};


////////////////////////////////////////////////////////////////////////////////

template <typename T>
class AsyncFrontend
{
	// AsyncFrontend interface
public:

	virtual bool clearFrontend()=0;
	virtual bool setFrontend(T data)=0;
	virtual T getFrontend(bool &ok)=0;
	virtual bool generateFrontend()=0;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
class AsyncStore
{
protected:

	AsyncBackend<T> &mBackend;
	AsyncFrontend<T> &mFrontend;

	//T mData;
	ConcurrentQueue<ASEvent<T> > mTransactions;

	QAtomicInteger<quint64> mAutoIncrement;
	QAtomicInteger<quint64> mDiskCounter;
	QAtomicInteger<quint64> mMemoryCounter;
	AtomicBoolean mDone;

	QFuture<void> mCompleteFuture;


public:
	explicit AsyncStore(AsyncBackend<T> &backend, AsyncFrontend<T> &frontend, QObject *parent = nullptr);
	virtual ~AsyncStore();

public:

	AsyncBackend<T> &backend();
	AsyncFrontend<T> &frontend();


public:

	QString filename() const;
	bool fileExists() const;
	bool ready();


private:

	ASEvent<T> enqueueTransaction(ASEvent<T> trans);
	void processEvents();

	void processEvent(ASEvent<T> &event);


	quint64 autoIncrement();

public:
	ASEvent<T> clear();
	ASEvent<T> get();
	ASEvent<T> set(T data);
	ASEvent<T> load();
	ASEvent<T> save();
	ASEvent<T> generate();
	ASEvent<T> synchronize();

private:

	ASEvent<T> complete();


private:

	// Delete data on disk and in memory
	bool clearSync();
	// Get data from memory
	T getSync(bool &ok);
	// Set data in memory
	bool setSync(T data);
	// Load data from file on disk into memory
	bool loadSync();
	// Save data from memory into file on disk
	bool saveSync();
	// Bootstrap data into memory
	bool generateSync();

	// Save, load  or bootstrap data depending on wether memory or disk is newest
	bool synchronizeSync();

	// Internal transaction to stop transaction processing thread
	bool completeSync();


public:

	friend class ASEvent<T>;
	friend class ASEventPrivate<T>;
	friend class SimpleDataStore;
	friend const QDebug &operator<< (QDebug &d, AsyncStore<T> &as);
};



////////////////////////////////////////////////////////////////////////////////






template <typename T>
AsyncStore<T>::AsyncStore(AsyncBackend<T> &backend, AsyncFrontend<T> &frontend, QObject *parent)
//: QObject(parent)
	: mBackend(backend)
	, mFrontend(frontend)
	, mAutoIncrement(0)
	, mDiskCounter(0)
	, mMemoryCounter(0)
	, mDone(false)
{
	OC_METHODGATE();
//	setObjectName("AsyncStore");
	const QString filename=mBackend.filenameBackend();
	const bool exists=mBackend.existsBackend();
	qDebug().noquote().nospace()<<"AsyncStore created () with filename="<< filename <<" (exists="<<exists<<") from "<<utility::currentThreadID();
	if(exists) {
		// Make sure that a sync from initial state will perform a load if there is data on disk
		mDiskCounter=autoIncrement();
	}
	// Start transaction processing in separate thread
	mCompleteFuture=QtConcurrent::run([this]() {
		OC_METHODGATE();
		//qDebug()<<"Entered AsyncStore::QtConcurrent::run::lambda() from thread "<<utility::currentThreadID();
		processEvents();
		//qDebug()<<"Exiting AsyncStore::QtConcurrent::run::lambda() from thread "<<utility::currentThreadID();
	});
}

template <typename T>
AsyncStore<T>::~AsyncStore()
{
	OC_METHODGATE();
	//qDebug()<<"Entered AsyncStore::~AsyncStore() from thread "<<utility::currentThreadID();
	// TODO: Look at how to avoid the possibility of an unecessary load during this final sync
	synchronize();
	complete();
	mCompleteFuture.waitForFinished();
	//qDebug()<<"Exiting AsyncStore::~AsyncStore() from thread "<<utility::currentThreadID();
}



template <typename T>
QString AsyncStore<T>::filename() const
{
	OC_METHODGATE();
	return mBackend.filenameBackend();
}

template <typename T>
bool AsyncStore<T>::fileExists() const
{
	OC_METHODGATE();
	return mBackend.existsBackend();
}

template <typename T>
bool AsyncStore<T>::ready()
{
	OC_METHODGATE();
	return mMemoryCounter > 0;
}


template <typename T>
ASEvent<T> AsyncStore<T>::enqueueTransaction(ASEvent<T> trans)
{
	OC_METHODGATE();
	//qDebug()<<"Entered AsyncStore::enqueueTransaction() from thread "<<utility::currentThreadID();
	mTransactions.put(trans);
	//qDebug()<<"Exiting AsyncStore::enqueueTransaction() from thread "<<utility::currentThreadID();
	return trans;
}

template <typename T>
void AsyncStore<T>::processEvents()
{
	OC_METHODGATE();
	//QThread::msleep(2000);
	//qDebug()<<"Entered AsyncStore::processTransactions() from thread "<<utility::currentThreadID();
	while(!mDone) {
		ASEvent<T> event=mTransactions.get();
		//qDebug()<<" + AsyncStore::processTransactions() running transaction with type="<<trans.type()<<"	from thread "<<utility::currentThreadID();
		processEvent(event);
		//qDebug()<<" + AsyncStore::processTransactions() done running transaction with type="<<trans.type()<<" from thread "<<utility::currentThreadID();
	}
	//qDebug()<<"Exiting AsyncStore::processTransactions() from thread "<<utility::currentThreadID();
}

template <typename T>
void AsyncStore<T>::processEvent(ASEvent<T> &event)
{
	OC_METHODGATE();
	event.run();
}

template <typename T>
quint64 AsyncStore<T>::autoIncrement()
{
	OC_METHODGATE();
	return ++mAutoIncrement;
}


template <typename T>
ASEvent<T> AsyncStore<T>::clear()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_CLEAR));
}

template <typename T>
ASEvent<T> AsyncStore<T>::get()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_GET));
}

template <typename T>
ASEvent<T> AsyncStore<T>::set(T data)
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_SET, data));
}

template <typename T>
ASEvent<T> AsyncStore<T>::load()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_LOAD));
}

template <typename T>
ASEvent<T> AsyncStore<T>::save()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_SAVE));
}

template <typename T>
ASEvent<T> AsyncStore<T>::generate()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_GENERATE));
}

template <typename T>
ASEvent<T> AsyncStore<T>::synchronize()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_SYNCHRONIZE));
}

template <typename T>
ASEvent<T> AsyncStore<T>::complete()
{
	OC_METHODGATE();
	return enqueueTransaction(ASEvent<T>(*this, AS_EVENT_DONE));
}

template <typename T>
bool AsyncStore<T>::clearSync()
{
	OC_METHODGATE();
	const bool backendOK=mBackend.clearBackend();
	//const auto oldD=mDiskCounter;
	//const auto oldM=mMemoryCounter;
	if(backendOK) {
		mDiskCounter=0;
	}
	const bool frontendOK=mFrontend.clearFrontend();
	if(frontendOK) {
		mMemoryCounter=0;
	}
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<oldM<<" to " <<mMemoryCounter<< " VIA CLEAR";
	//qDebug()<<"DISK COUNTER FOR "<<mFilename<<" WENT FROM "<<oldD<<" to " <<mDiskCounter<< " VIA CLEAR";
	return backendOK && frontendOK;
}



template <typename T>
T AsyncStore<T>::getSync(bool &ok)
{
	OC_METHODGATE();
	qDebug()<<"Entering Sync Get from "<<utility::currentThreadID();
	ok=false;
	T data = mFrontend.getFrontend(ok);
	qDebug()<<"Exiting Sync Get with ok="<<ok<<" and data="<<data<<" from "<<utility::currentThreadID();
	return data;
}


template <typename T>
bool AsyncStore<T>::setSync(T data)
{
	OC_METHODGATE();
	qDebug()<<"Entering Sync Set with data="<<data<<" from "<<utility::currentThreadID();
	const  bool ok=mFrontend.setFrontend(data);
	//const auto old=mMemoryCounter;
	if(ok) {
		mMemoryCounter=autoIncrement();
	}
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT";
	qDebug()<<"Exiting Sync Set with ok="<<ok<<" from "<<utility::currentThreadID();
	return ok;
}

template <typename T>
bool AsyncStore<T>::loadSync()
{
	OC_METHODGATE();
	qDebug()<<"Entering Sync Load from "<<utility::currentThreadID();
	bool ok=false;
	T data = mBackend.loadBackend(ok);
	if(ok) {
		ok = mFrontend.setFrontend(data);
		if(ok) {
			mMemoryCounter = mDiskCounter;
		}
	}
	qDebug()<<"Exiting Sync Load with ok="<<ok<<"  from "<<utility::currentThreadID();
	return ok;
}

template <typename T>
bool AsyncStore<T>::saveSync()
{
	OC_METHODGATE();
	qDebug()<<"Entering Sync Save from "<<utility::currentThreadID();
	bool ok=false;
	T data = mFrontend.getFrontend(ok);
	if(ok) {
		ok = mBackend.saveBackend(data);
		if(ok) {
			mDiskCounter = mMemoryCounter;
		}
	}
	qDebug()<<"Exiting Sync Save with ok="<<ok<<"  from "<<utility::currentThreadID();
	return ok;
}


// Bootstrap data into memory
template <typename T>
bool AsyncStore<T>::generateSync()
{
	OC_METHODGATE();
	qDebug()<<"Entering Sync Generate from "<<utility::currentThreadID();
	const bool ok=mFrontend.generateFrontend();
	//const auto old=mMemoryCounter;
	if(ok) {
		mMemoryCounter=autoIncrement();
	}
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT";
	qDebug()<<"Exiting Sync Generate with ok="<<ok<<"  from "<<utility::currentThreadID();
	return ok;
}

template <typename T>
bool AsyncStore<T>::synchronizeSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Synchronize";
	bool ok=false;
	const quint64 disk=mDiskCounter;
	const quint64 mem= mMemoryCounter;
	qDebug()<<"AsyncStore::synchronizeSync("<<filename()<<"): disk("<<disk<<") == mem("<<mem<<")  from "<<utility::currentThreadID();
	if(disk == mem) {
		if(0 == disk) {
			ok=generateSync();
			qDebug()<<" + generate:"<<ok<<"  from "<<utility::currentThreadID();
		} else {
			ok=true;
			qDebug()<<" + no-op:"<<ok<<"  from "<<utility::currentThreadID();
		}
	} else if(disk > mem) {
		ok=loadSync();
		qDebug()<<" + load:"<<ok<<"  from "<<utility::currentThreadID();
	} else if(disk < mem) {
		ok=saveSync();
		qDebug()<<" + save:"<<ok<<"  from "<<utility::currentThreadID();
	}
	return ok;
}

template <typename T>
bool AsyncStore<T>::completeSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Completion";
	bool ok=true;
	mDone=true;
	//qDebug()<<"Exiting Sync Completion with ok="<<ok;
	return ok;
}


template <typename T>
const QDebug &operator<<(QDebug &d, AsyncStore<T> &as)
{
	OC_FUNCTIONGATE();
	d.nospace() <<"AsyncStore{ fn="<<as.mFilename;
//	d.nospace() <<", fexists="<<ks.fileExists()<<", ready="<<(const bool)ks.mLoadCompleted<<", inProgress="<<(const bool)ks.mLoadInProgress<<", error="<<(const bool)ks.mLoadHadErrors<<", config:{";
	d.nospace() << as.mData;
	d.nospace() <<"}";
	return d.maybeSpace();
}





#endif // ASYNCSTORE_HPP
