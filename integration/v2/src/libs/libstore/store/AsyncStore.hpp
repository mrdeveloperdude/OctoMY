#ifndef ASYNCSTORE_HPP
#define ASYNCSTORE_HPP

#include "uptime/New.hpp"

#include "utility/concurrent/ConcurrentQueue.hpp"
#include "AsyncStoreStatus.hpp"
#include "ASEvent.hpp"
#include "uptime/SharedPointerWrapper.hpp"
//#include "node/DataStoreInterface.hpp"


#include <QByteArray>
#include <QDebug>
#include <QJsonParseError>
#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QWaitCondition>

#include <QMutex>
#include <QMutexLocker>

/*

  AsyncStore is used to persist data in form of variant maps.

  The long term goal of the API is to ensure compliance with ACID requirements as far as
  possible while also being asynchronous.

  The API should also be as transparent as possible, meaning that using it should
  require the least possible amount of boilerplate.

  ACID is an acronym for the following 4 traits:

  + Atomicity    - The operation is indivisible, all-or-nothing. A transaction can be "committed" or "rolled back"; if it is committed all of its effects must complete fully, otherwise if it is rolled back it must have no effect.
  + Consistency  - The transaction transforms the database from one consistent state to another consistent state. Here "consistent" implies obeying not just relational integrity constraints but also business rules and semantics, such as that a given Customer record's Address and ZIP fields must not specify inconsistent information (e.g., an address in New York and a ZIP code in Alabama).
  + Isolation    - Two transactions are fully isolated if they can be started simultaneously and the result is guaranteed to be the same as though one was run entirely before the other ("Serializable"). In practice, databases support various levels of isolation, each of which trades off better isolation at the expense of concurrency or vice versa. Some typical isolation levels, in order from best to worst concurrency, are: Read Uncommitted (a.k.a. Dirty Read), Read Committed, Repeatable Read, and Serializable (highest isolation), and describe what kinds of intermediate states of other transactions might be visible.
  + Durability   - If a transaction is committed, its effects are not lost even if there is a subsequent system crash. This requirement is usually implemented using a transaction log which is used to "roll forward" the state of the database on system restart.

*/


////////////////////////////////////////////////////////////////////////////////

// The backend is the synchronous part of the API that actually carries out the
// storage operations.


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

// The frontend is the asynchronous interface to the backend.

template <typename T>
class AsyncFrontend
{
	// AsyncFrontend interface
public:

	virtual bool clearFrontend() = 0;
	virtual bool setFrontend(T data) = 0;
	virtual T getFrontend(bool &ok) = 0;
	virtual bool generateFrontend() = 0;
};

////////////////////////////////////////////////////////////////////////////////



template <typename T>
class AsyncStore
{
protected:
	QSharedPointer<QObject> mSignallingObject;

	QSharedPointer<AsyncBackend<T> > mBackend;
	QSharedPointer<AsyncFrontend<T> > mFrontend;

	//T mData;
	ConcurrentQueue<ASEvent<T> > mTransactions;

	QAtomicInteger<quint64> mAutoIncrement;
	QAtomicInteger<quint64> mDiskCounter;
	QAtomicInteger<quint64> mMemoryCounter;
	AtomicBoolean mCompleted;

	QFuture<void> mCompleteFuture;

	QMutex mJournalMutex;
	QStringList mJournal;

	AtomicBoolean mSynchronousMode;

public:
	explicit AsyncStore(QObject *parent = nullptr);
	virtual ~AsyncStore();

public:
	void configure(QSharedPointer<AsyncBackend<T> > backend, QSharedPointer<AsyncFrontend<T> > frontend);

private:
	void addJournal(QString);

public:
	QStringList journal();

	void setSynchronousMode(bool isSync);

public:

	AsyncBackend<T> &backend();
	AsyncFrontend<T> &frontend();

public:

	QString filename() const;
	bool fileExists() const;
	bool ready();


private:

	virtual ASEvent<T> enqueueEvent(ASEvent<T> trans);
	virtual void processEvents();

	virtual void processEvent(ASEvent<T> &event);
	quint64 autoIncrement();

	void runCallbacksForEvent(ASEvent<T> event);


	// The asynchronous api
public:
	ASEvent<T> status();
	ASEvent<T> clear();
	ASEvent<T> get();
	ASEvent<T> set(T data);
	ASEvent<T> load();
	ASEvent<T> save();
	ASEvent<T> generate();
	ASEvent<T> synchronize();

private:

	ASEvent<T> complete();

	// The synchronous actions
private:

	// Get current status
	AsyncStoreStatus statusSync();
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
	template <typename F>
	friend const QDebug &operator<< (QDebug &d, AsyncStore<F> &as);
};



////////////////////////////////////////////////////////////////////////////////




template <typename T>
AsyncStore<T>::AsyncStore(QObject *parent)
//: QObject(parent)
	: mSignallingObject(OC_NEW QObject())
	, mAutoIncrement(0)
	, mDiskCounter(0)
	, mMemoryCounter(0)
	, mCompleted(false)
	, mSynchronousMode(false)
{
	OC_METHODGATE();
	// TODO: Remove the parent parameter as it is not in use
	Q_UNUSED(parent);
}

template <typename T>
AsyncStore<T>::~AsyncStore()
{
	OC_METHODGATE();
	//qDebug()<<"Entered AsyncStore::~AsyncStore() from thread "<<utility::concurrent::currentThreadID();
	// TODO: Look at how to avoid the possibility of an unecessary load during this final sync
	qDebug()<<"D-TOR";
	synchronize();
	complete();
	mCompleteFuture.waitForFinished();
	mSignallingObject->deleteLater();
	mSignallingObject=nullptr;
	qDebug()<<"D-TOR JOURNAL: "<<journal();
	//qDebug()<<"Exiting AsyncStore::~AsyncStore() from thread "<<utility::concurrent::currentThreadID();
}


template <typename T>
void AsyncStore<T>::configure(QSharedPointer<AsyncBackend<T> > backend, QSharedPointer<AsyncFrontend<T> > frontend)
{
	OC_METHODGATE();
	mBackend=backend;
	mFrontend=frontend;

	if(!mBackend.isNull()) {
		const QString filename=mBackend->filenameBackend();
		const bool exists=mBackend->existsBackend();
		//qDebug().noquote().nospace()<<"AsyncStore created () with filename="<< filename <<" (exists="<<exists<<") from "<<utility::concurrent::currentThreadID();
		if(exists) {
			// Make sure that a sync from initial state will perform a load if there is data on disk
			mDiskCounter=autoIncrement();
		}
		// Start transaction processing in separate thread
		mCompleteFuture=QtConcurrent::run([this]() {
			OC_METHODGATE();
			//qDebug()<<"Entered AsyncStore::QtConcurrent::run::lambda() from thread "<<utility::concurrent::currentThreadID();
			processEvents();
			//qDebug()<<"Exiting AsyncStore::QtConcurrent::run::lambda() from thread "<<utility::concurrent::currentThreadID();
		});
	} else {
		qWarning()<<"ERROR: no backend";
	}
}

template <typename T>
void AsyncStore<T>::addJournal(QString entry)
{
	OC_METHODGATE();
	QMutexLocker ml(&mJournalMutex);
	mJournal << entry;
}

template <typename T>
QStringList AsyncStore<T>::journal()
{
	OC_METHODGATE();
	QMutexLocker ml(&mJournalMutex);
	return mJournal;
}

template <typename T>
void AsyncStore<T>::setSynchronousMode(bool isSync)
{
	OC_METHODGATE();
	mSynchronousMode.set(isSync);
}


template <typename T>
QString AsyncStore<T>::filename() const
{
	OC_METHODGATE();
	return (mBackend.isNull())?"":mBackend->filenameBackend();
}

template <typename T>
bool AsyncStore<T>::fileExists() const
{
	OC_METHODGATE();
	return (mBackend.isNull())?false:mBackend->existsBackend();
}

template <typename T>
bool AsyncStore<T>::ready()
{
	OC_METHODGATE();
	const quint64 value=mMemoryCounter;
	return value > 0;
}

template <typename T>
ASEvent<T> AsyncStore<T>::enqueueEvent(ASEvent<T> event)
{
	OC_METHODGATE();
	// We are not asynchronous, do events as they occur
	if(mSynchronousMode) {
		processEvent(event);
	}
	// We are asynchronous, post events to log and return
	else {
		//qDebug()<<"Entered AsyncStore::enqueueTransaction() from thread "<<utility::concurrent::currentThreadID();
		mTransactions.put(event);
		//qDebug()<<"Exiting AsyncStore::enqueueTransaction() from thread "<<utility::concurrent::currentThreadID();
	}
	return event;
}

template <typename T>
void AsyncStore<T>::processEvents()
{
	OC_METHODGATE();
	//QThread::msleep(2000);
	//qDebug()<<"Entered AsyncStore::processTransactions() from thread "<<utility::concurrent::currentThreadID();
	while(!mCompleted && !mSynchronousMode) {
		ASEvent<T> event=mTransactions.get();
		//qDebug()<<" + AsyncStore::processTransactions() running transaction with type="<<trans.type()<<"	from thread "<<utility::concurrent::currentThreadID();
		processEvent(event);
		//qDebug()<<" + AsyncStore::processTransactions() done running transaction with type="<<trans.type()<<" from thread "<<utility::concurrent::currentThreadID();
	}
	//qDebug()<<"Exiting AsyncStore::processTransactions() from thread "<<utility::concurrent::currentThreadID();
}

template <typename T>
void AsyncStore<T>::processEvent(ASEvent<T> &event)
{
	OC_METHODGATE();
	//qDebug()<<"Processing event "<<event;
	event.run();
}

template <typename T>
quint64 AsyncStore<T>::autoIncrement()
{
	OC_METHODGATE();
	return ++mAutoIncrement;
}


template <typename T>
void AsyncStore<T>::runCallbacksForEvent(ASEvent<T> event)
{
	OC_METHODGATE();
	ASEvent<T> *copy=OC_NEW ASEvent<T>(event);
	//auto p=event.p();
	//qDebug()<<"runCallbacksForEvent() from "<<utility::concurrent::currentThreadID();
	if(nullptr!=copy) {
		utility::concurrent::postToThread([copy] {
			//qDebug()<<"CALLBACKS SINGLESHOT from "<<utility::concurrent::currentThreadID();
			//ASEvent<T> e(p);
			//e.runCallbacks();
			if(nullptr!=copy)
			{
				copy->runCallbacks();
				delete copy;
				//copy=nullptr;
			}
		});
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//  Asynchronous actions
//


template <typename T>
ASEvent<T> AsyncStore<T>::status()
{
	OC_METHODGATE();
	addJournal("add-status");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_STATUS));
}


template <typename T>
ASEvent<T> AsyncStore<T>::clear()
{
	OC_METHODGATE();
	addJournal("add-clear");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_CLEAR));
}


template <typename T>
ASEvent<T> AsyncStore<T>::get()
{
	OC_METHODGATE();
	addJournal("add-get");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_GET));
}

template <typename T>
ASEvent<T> AsyncStore<T>::set(T data)
{
	OC_METHODGATE();
	addJournal("add-set");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_SET, data));
}

template <typename T>
ASEvent<T> AsyncStore<T>::load()
{
	OC_METHODGATE();
	addJournal("add-load");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_LOAD));
}

template <typename T>
ASEvent<T> AsyncStore<T>::save()
{
	OC_METHODGATE();
	addJournal("add-save");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_SAVE));
}

template <typename T>
ASEvent<T> AsyncStore<T>::generate()
{
	OC_METHODGATE();
	addJournal("add-generate");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_GENERATE));
}

template <typename T>
ASEvent<T> AsyncStore<T>::synchronize()
{
	OC_METHODGATE();
	addJournal("add-sync");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_SYNCHRONIZE));
}

template <typename T>
ASEvent<T> AsyncStore<T>::complete()
{
	OC_METHODGATE();
	addJournal("add-complete");
	return enqueueEvent(ASEvent<T>(*this, AS_EVENT_COMPLETE));
}


////////////////////////////////////////////////////////////////////////////////
//
//  Synchronous actions
//


template <typename T>
AsyncStoreStatus AsyncStore<T>::statusSync()
{
	OC_METHODGATE();
	addJournal("status");
	return AsyncStoreStatus(mDiskCounter, mMemoryCounter);
}


template <typename T>
bool AsyncStore<T>::clearSync()
{
	OC_METHODGATE();
	const bool backendOK=mBackend.isNull()?false:mBackend->clearBackend();
	//const auto oldD=mDiskCounter;
	//const auto oldM=mMemoryCounter;
	if(backendOK) {
		mDiskCounter=0;
	}
	const bool frontendOK=mFrontend.isNull()?false:mFrontend->clearFrontend();
	if(frontendOK) {
		mMemoryCounter=0;
	}
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<oldM<<" to " <<mMemoryCounter<< " VIA CLEAR";
	//qDebug()<<"DISK COUNTER FOR "<<mFilename<<" WENT FROM "<<oldD<<" to " <<mDiskCounter<< " VIA CLEAR";
	addJournal("clear");
	return backendOK && frontendOK;
}



template <typename T>
T AsyncStore<T>::getSync(bool &ok)
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Get from "<<utility::concurrent::currentThreadID();
	ok=false;
	T data = mFrontend.isNull()?T():mFrontend->getFrontend(ok);
	//qDebug()<<"Exiting Sync Get with ok="<<ok<<" and data="<<data<<" from "<<utility::concurrent::currentThreadID();
	addJournal("get");
	return data;
}


template <typename T>
bool AsyncStore<T>::setSync(T data)
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Set with data="<<data<<" from "<<utility::concurrent::currentThreadID();
	const  bool ok=mFrontend.isNull()?false:mFrontend->setFrontend(data);
	//const auto old=mMemoryCounter;
	if(ok) {
		mMemoryCounter=autoIncrement();
	}
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT";
	//qDebug()<<"Exiting Sync Set with ok="<<ok<<" from "<<utility::concurrent::currentThreadID();
	addJournal("set");
	return ok;
}

template <typename T>
bool AsyncStore<T>::loadSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Load from "<<utility::concurrent::currentThreadID();
	bool ok=false;
	T data = mBackend.isNull()?T():mBackend->loadBackend(ok);
	if(ok) {
		ok = mFrontend.isNull()?false:mFrontend->setFrontend(data);
		if(ok) {
			mMemoryCounter = mDiskCounter;
		}
	}
	//qDebug()<<"Exiting Sync Load with ok="<<ok<<"  from "<<utility::concurrent::currentThreadID();
	addJournal("load");
	return ok;
}

template <typename T>
bool AsyncStore<T>::saveSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Save from "<<utility::concurrent::currentThreadID();
	bool ok=false;
	T data = mFrontend.isNull()?T():mFrontend->getFrontend(ok);
	if(ok) {
		ok = mBackend->saveBackend(data);
		if(ok) {
			mDiskCounter = mMemoryCounter;
		}
	}
	//qDebug()<<"Exiting Sync Save with ok="<<ok<<"  from "<<utility::concurrent::currentThreadID();
	addJournal("save");
	return ok;
}


// Bootstrap data into memory
template <typename T>
bool AsyncStore<T>::generateSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Generate from "<<utility::concurrent::currentThreadID();
	const bool ok=mFrontend.isNull()?false:mFrontend->generateFrontend();
	//const auto old=mMemoryCounter;
	if(ok) {
		mMemoryCounter=autoIncrement();
	}
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT";
	//qDebug()<<"Exiting Sync Generate with ok="<<ok<<"  from "<<utility::concurrent::currentThreadID();
	addJournal("generate");
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
	//qDebug()<<"AsyncStore::synchronizeSync("<<filename()<<"): disk("<<disk<<") == mem("<<mem<<")  from "<<utility::concurrent::currentThreadID();

	if(disk > mem) {
		ok=loadSync();
		//qDebug()<<" + load:"<<ok<<"  from "<<utility::concurrent::currentThreadID();
	} else if(disk < mem) {
		ok=saveSync();
		//qDebug()<<" + save:"<<ok<<"  from "<<utility::concurrent::currentThreadID();
	}
	// equal
	else {
		if(0 == disk) {
			ok=generateSync() && saveSync();
			//qDebug()<<" + generate:"<<ok<<"  from "<<utility::concurrent::currentThreadID();
		} else {
			ok=true;
			//qDebug()<<" + no-op:"<<ok<<"  from "<<utility::concurrent::currentThreadID();
		}
	}
	addJournal("sync");
	return ok;
}

template <typename T>
bool AsyncStore<T>::completeSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Completion";
	bool ok=true;
	mCompleted=true;
	//qDebug()<<"Exiting Sync Completion with ok="<<ok;
	addJournal("complete");
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

#endif
// ASYNCSTORE_HPP
