#ifndef ASYNCSTORE_HPP
#define ASYNCSTORE_HPP

#include "uptime/New.hpp"

#include "utility/concurrent/ConcurrentQueue.hpp"
#include "AsyncStoreStatus.hpp"
#include "ASEvent.hpp"
#include "uptime/SharedPointerWrapper.hpp"
//#include "node/DataStoreInterface.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "app/Constants.hpp"

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

	ConcurrentQueue<ASEvent<T> > mTransactions;

	QAtomicInteger<quint64> mAutoIncrement;
	QAtomicInteger<quint64> mDiskCounter;
	QAtomicInteger<quint64> mMemoryCounter;
	AtomicBoolean mCompleted;

	QFuture<void> mCompleteFuture;

	QMutex mJournalMutex;
	QStringList mJournal;

	AtomicBoolean mSynchronousMode;

	ConfigureHelper mConfigureHelper;

public:
	explicit AsyncStore();
	virtual ~AsyncStore();

	// ConfigureHelper interface
public:
	void configure(QSharedPointer<AsyncBackend<T> > backend, QSharedPointer<AsyncFrontend<T> > frontend);
	void activate(const bool on, std::function<void(bool)> callBack=nullptr);

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
	ASEvent<T> enqueueEvent(ASEvent<T> trans);
	void processEvents();
	void listEvents();

	void processEvent(ASEvent<T> &event);
	quint64 autoIncrement();

	void setMemoryCounter(quint64 newValue);
	void setDiskCounter(quint64 newValue);

	void runCallbacksForEvent(ASEvent<T> event, bool runCopy=true);

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
AsyncStore<T>::AsyncStore()
	: mSignallingObject(OC_NEW QObject())
	, mAutoIncrement(0)
	, mDiskCounter(0)
	, mMemoryCounter(0)
	, mCompleted(false)
	, mSynchronousMode(false)
	, mConfigureHelper("AsyncStore", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
{
	OC_METHODGATE();
}

template <typename T>
AsyncStore<T>::~AsyncStore()
{
	OC_METHODGATE();
	//qDebug()<<"Entered AsyncStore::~AsyncStore() from thread "<<utility::concurrent::currentThreadID();
	// TODO: Look at how to avoid the possibility of an unecessary load during this final sync
	qDebug()<<"D-TOR";
	mSignallingObject->deleteLater();
	mSignallingObject=nullptr;
	qDebug()<<"D-TOR JOURNAL: "<<journal();
	//qDebug()<<"Exiting AsyncStore::~AsyncStore() from thread "<<utility::concurrent::currentThreadID();
}


template <typename T>
void AsyncStore<T>::configure(QSharedPointer<AsyncBackend<T> > backend, QSharedPointer<AsyncFrontend<T> > frontend)
{
	OC_METHODGATE();
	addJournal("CONFIG");
	if(mConfigureHelper.configure()) {
		mBackend=backend;
		mFrontend=frontend;
	}
}


template <typename T>
void AsyncStore<T>::activate(const bool on, std::function<void(bool)> callBack)
{
	OC_METHODGATE();
	on?addJournal("ACTIVATE ON"):addJournal("ACTIVATE OFF");

	if(on) {
		if(mConfigureHelper.activate(on)) {
			if(!mBackend.isNull()) {
				//const QString filename=mBackend->filenameBackend();
				const bool exists=mBackend->existsBackend();
				//qDebug().noquote().nospace()<<"AsyncStore created () with filename="<< filename <<" (exists="<<exists<<") from "<<utility::concurrent::currentThreadID();
				if(exists) {
					// Make sure that a sync from initial state will perform a load if there is data on disk
					setDiskCounter(autoIncrement());
				}
			} else {
				qWarning()<<"ERROR: no backend";
			}
			// NOTE: We activate the transactions queue here, but we don't deactivate it until the "completed" operation is received
			mTransactions.activate(on);
			// Start transaction processing in separate thread
			mCompleteFuture=QtConcurrent::run([this]() {
				OC_METHODGATE();
				qDebug()<<"Entered AsyncStore::QtConcurrent::run::lambda() from thread "<<utility::concurrent::currentThreadID();
				processEvents();
				qDebug()<<"Exiting AsyncStore::QtConcurrent::run::lambda() from thread "<<utility::concurrent::currentThreadID();
				listEvents();
			});
			/*
			 * NOTE: This was moved to SimpleDataStore and KeyStore as that is more appropriate location
			// Synchronize to get started
			synchronize().onFinished([callBack](ASEvent<T> &ase) {
				if(nullptr!=callBack) {
					callBack(ase.isSuccessfull());
				}
			});
			*/
			if(nullptr!=callBack) {
				callBack(true);
			}
		}
	} else {
		synchronize();
		complete();
		qDebug()<<" #-#-# Waiting for asyncstore future finish: running="<< mCompleteFuture.isRunning() << ", started="<< mCompleteFuture.isStarted() << ", paused="<< mCompleteFuture.isPaused() << ", canceled="<< mCompleteFuture.isCanceled();
		mCompleteFuture.waitForFinished();
		qDebug()<<" #-#-# Got asyncstore future finish: running="<< mCompleteFuture.isRunning() << ", started="<< mCompleteFuture.isStarted() << ", paused="<< mCompleteFuture.isPaused() << ", canceled="<< mCompleteFuture.isCanceled();

		if(nullptr!=callBack) {
			callBack(true);
		}
		// NOTE: synchronize and complete both need store to be activated, so we deactivate last
		mConfigureHelper.activate(on);
	}
}

template <typename T>
void AsyncStore<T>::addJournal(QString entry)
{
	OC_METHODGATE();
	QMutexLocker ml(&mJournalMutex);
	//qDebug()<<"Adding to journal: "<<entry;
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
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return (mBackend.isNull())?"":mBackend->filenameBackend();
	} else {
		return "";
	}
}

template <typename T>
bool AsyncStore<T>::fileExists() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return (mBackend.isNull())?false:mBackend->existsBackend();
	} else {
		return false;
	}
}

template <typename T>
bool AsyncStore<T>::ready()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		const quint64 value=mMemoryCounter;
		return value > 0;
	} else {
		return false;
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::enqueueEvent(ASEvent<T> event)
{
	OC_METHODGATE();
	//qDebug()<<"Enqueuing event "<<event;
	// We are not asynchronous, do events as they occur
	if(mSynchronousMode) {
		processEvent(event);
	}
	// We are asynchronous, post events to log and return
	else {
		// qDebug()<<"Entered AsyncStore::enqueueTransaction() from thread "<<utility::concurrent::currentThreadID();
		mTransactions.put(event);
		// qDebug()<<"Exiting AsyncStore::enqueueTransaction() from thread "<<utility::concurrent::currentThreadID();
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
void AsyncStore<T>::listEvents()
{
	OC_METHODGATE();
	mTransactions.list();
}


template <typename T>
void AsyncStore<T>::processEvent(ASEvent<T> &event)
{
	OC_METHODGATE();
	//qDebug()<<"Processing event "<<event;
	if(!event.isNull()) {
		event.run();
	}
}

template <typename T>
quint64 AsyncStore<T>::autoIncrement()
{
	OC_METHODGATE();
	return ++mAutoIncrement;
}


template <typename T>
void AsyncStore<T>::setDiskCounter(quint64 newValue)
{
	OC_METHODGATE();
	const auto oldValue=mDiskCounter;
	mDiskCounter=newValue;
	addJournal("set-disk="+QString::number(mDiskCounter));
	qDebug()<<"COUNTERS FOR "<<filename()<<":";
	qDebug()<<" MEM COUNTER IS  " <<mMemoryCounter;
	qDebug()<<"DISK COUNTER IS  " <<mDiskCounter<<" (FROM "<<oldValue<<")";
	qDebug()<<"AUTO COUNTER IS  " <<mAutoIncrement;
	qDebug()<<"	    JOURNAL IS  "<<journal();

}


template <typename T>
void AsyncStore<T>::setMemoryCounter(quint64 newValue)
{
	OC_METHODGATE();
	const auto oldValue=mMemoryCounter;
	mMemoryCounter=newValue;
	addJournal("set-mem="+QString::number(mMemoryCounter));
	qDebug()<<"COUNTERS FOR "<<filename()<<":";
	qDebug()<<" MEM COUNTER IS  " <<mMemoryCounter<<" (FROM "<<oldValue<<")";
	qDebug()<<"DISK COUNTER IS  " <<mDiskCounter;
	qDebug()<<"AUTO COUNTER IS  " <<mAutoIncrement;
	qDebug()<<"	    JOURNAL IS  "<<journal();
}


template <typename T>
void AsyncStore<T>::runCallbacksForEvent(ASEvent<T> origEvent, bool runCopy)
{
	OC_METHODGATE();
	ASEvent<T> *event=runCopy?(OC_NEW ASEvent<T>(origEvent)):(&origEvent);
	//auto p=event.p();
	//qDebug()<<"runCallbacksForEvent() from "<<utility::concurrent::currentThreadID();
	if(nullptr!=event) {
		utility::concurrent::postToThread([event] {
			//qDebug()<<"CALLBACKS SINGLESHOT from "<<utility::concurrent::currentThreadID();
			//ASEvent<T> e(p);
			//e.runCallbacks();
			if(nullptr!=event)
			{
				event->runCallbacks();
				delete event;
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
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-status");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_STATUS));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}


template <typename T>
ASEvent<T> AsyncStore<T>::clear()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-clear");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_CLEAR));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}


template <typename T>
ASEvent<T> AsyncStore<T>::get()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-get");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_GET));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::set(T data)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-set");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_SET, data));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::load()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-load");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_LOAD));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::save()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-save");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_SAVE));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::generate()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-generate");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_GENERATE));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::synchronize()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-sync");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_SYNCHRONIZE));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}

template <typename T>
ASEvent<T> AsyncStore<T>::complete()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		addJournal("add-complete");
		return enqueueEvent(ASEvent<T>(*this, AS_EVENT_COMPLETE));
	} else {
		return ASEvent<T>(*this, AS_EVENT_NONE);
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//  Synchronous actions
//


template <typename T>
AsyncStoreStatus AsyncStore<T>::statusSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		auto status=AsyncStoreStatus(mDiskCounter, mMemoryCounter);
		addJournal(QString("status=%1").arg(status.toString()));
		return status;
	} else {
		return AsyncStoreStatus(0, 0);
	}

}


template <typename T>
bool AsyncStore<T>::clearSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		const bool backendOK=mBackend.isNull()?false:mBackend->clearBackend();
		//const auto oldD=mDiskCounter;
		//const auto oldM=mMemoryCounter;
		if(backendOK) {
			setDiskCounter(0);
		}
		const bool frontendOK=mFrontend.isNull()?false:mFrontend->clearFrontend();
		if(frontendOK) {
			setMemoryCounter(0);
		}
		//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<oldM<<" to " <<mMemoryCounter<< " VIA CLEAR";
		//qDebug()<<"DISK COUNTER FOR "<<mFilename<<" WENT FROM "<<oldD<<" to " <<mDiskCounter<< " VIA CLEAR";
		addJournal(QString("clear_B=%1_F=%2").arg(backendOK?"ok":"fail").arg(frontendOK?"ok":"fail"));
		return backendOK && frontendOK;
	} else {
		return false;
	}
}



template <typename T>
T AsyncStore<T>::getSync(bool &ok)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		//qDebug()<<"Entering Sync Get from "<<utility::concurrent::currentThreadID();
		ok=false;
		T data = mFrontend.isNull()?T():mFrontend->getFrontend(ok);
		//qDebug()<<"Exiting Sync Get with ok="<<ok<<" and data="<<data<<" from "<<utility::concurrent::currentThreadID();
		addJournal(QString("get=%1").arg(ok?"ok":"fail"));
		return data;
	} else {
		return T();
	}
}


template <typename T>
bool AsyncStore<T>::setSync(T data)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		//qDebug()<<"Entering Sync Set with data="<<data<<" from "<<utility::concurrent::currentThreadID();
		const  bool ok=mFrontend.isNull()?false:mFrontend->setFrontend(data);
		//const auto old=mMemoryCounter;
		if(ok) {
			setMemoryCounter(autoIncrement());
		}
		//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT";
		//qDebug()<<"Exiting Sync Set with ok="<<ok<<" from "<<utility::concurrent::currentThreadID();
		addJournal(QString("set=%1").arg(ok?"ok":"fail"));
		return ok;
	} else {
		return false;
	}
}

template <typename T>
bool AsyncStore<T>::loadSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		//qDebug()<<"Entering Sync Load from "<<utility::concurrent::currentThreadID();
		bool ok=false;
		T data = mBackend.isNull()?T():mBackend->loadBackend(ok);
		if(ok) {
			ok = mFrontend.isNull()?false:mFrontend->setFrontend(data);
			if(ok) {
				setMemoryCounter(mDiskCounter);
			}
		}
		//qDebug()<<"Exiting Sync Load with ok="<<ok<<"  from "<<utility::concurrent::currentThreadID();
		addJournal(QString("load=%1").arg(ok?"ok":"fail"));
		return ok;
	} else {
		return false;
	}
}

template <typename T>
bool AsyncStore<T>::saveSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		//qDebug()<<"Entering Sync Save from "<<utility::concurrent::currentThreadID();
		bool ok=false;
		T data = mFrontend.isNull()?T():mFrontend->getFrontend(ok);
		if(ok) {
			ok = mBackend->saveBackend(data);
			if(ok) {
				setDiskCounter(mMemoryCounter);
			}
		}
		//qDebug()<<"Exiting Sync Save with ok="<<ok<<"  from "<<utility::concurrent::currentThreadID();
		addJournal(QString("save=%1").arg(ok?"ok":"fail"));
		return ok;
	} else {
		return false;
	}
}


// Bootstrap data into memory
template <typename T>
bool AsyncStore<T>::generateSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		//qDebug()<<"Entering Sync Generate from "<<utility::concurrent::currentThreadID();
		const bool ok=mFrontend.isNull()?false:mFrontend->generateFrontend();
		const auto old=mMemoryCounter;
		if(ok) {
			setMemoryCounter(autoIncrement());
		}
		//qDebug()<<"MEMORY COUNTER FOR "<<filename()<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT ("<<(ok?"ok":"fail")<<")";
		//qDebug()<<"Exiting Sync Generate with ok="<<ok<<"  from "<<utility::concurrent::currentThreadID();
		addJournal(QString("generate=%1").arg(ok?"ok":"fail"));
		return ok;
	} else {
		return false;
	}
}

template <typename T>
bool AsyncStore<T>::synchronizeSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
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
				ok=generateSync();
				ok=ok && saveSync();
				//qDebug()<<" + generate:"<<ok<<"  from "<<utility::concurrent::currentThreadID();
			} else {
				ok=true;
				//qDebug()<<" + no-op:"<<ok<<"  from "<<utility::concurrent::currentThreadID();
			}
		}
		addJournal(QString("sync=%1").arg(ok?"ok":"fail"));
		return ok;
	} else {
		return false;
	}
}

template <typename T>
bool AsyncStore<T>::completeSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		qDebug()<<"Entering Sync Completion";
		bool ok=true;
		mCompleted=true;
		qDebug()<<"Exiting Sync Completion with ok="<<ok;
		addJournal(QString("complete=%1").arg(ok?"ok":"fail"));
		mTransactions.activate(false);
		return ok;
	} else {
		return false;
	}
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
