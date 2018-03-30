#include "DataStore.hpp"

#include "pose/PoseMapping.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QJsonParseError>
#include <QByteArray>
#include <QMap>


QDebug operator<< (QDebug d, StorageEventType tt)
{
	OC_FUNCTIONGATE();
	switch(tt) {
#define TransactionType_case_stanza(A) case(A):{ d.nospace() << #A; }break
		TransactionType_case_stanza(STORAGE_EVENT_CLEAR);
		TransactionType_case_stanza(STORAGE_EVENT_GET);
		TransactionType_case_stanza(STORAGE_EVENT_SET);
		TransactionType_case_stanza(STORAGE_EVENT_LOAD);
		TransactionType_case_stanza(STORAGE_EVENT_SAVE);
		TransactionType_case_stanza(STORAGE_EVENT_SYNCHRONIZE);
		TransactionType_case_stanza(STORAGE_EVENT_DONE);
	default: {
		d.nospace() << "UNKNOWN";
	}
	break;
#undef TransactionType_case_stanza
	}

	return d.space();
}

static quint64 handleCounter=0;
static QMap<Qt::HANDLE, quint64> handleMap;
static QString handleCounterString(Qt::HANDLE h)
{
	if(nullptr==h) {
		return "H-null";
	}
	if(!handleMap.contains(h)) {
		handleCounter++;
		handleMap.insert(h,handleCounter);
	}
	return "H-"+QString::number(handleMap[h]);
}


static quint64 privCounter=0;
static QMap<const StorageEventPrivate *, quint64> privMap;
static QString privCounterString(const StorageEventPrivate *p)
{
	if(nullptr==p) {
		return "P-null";
	}
	if(!privMap.contains(p)) {
		privCounter++;
		privMap.insert(p, privCounter);
	}
	return "P-"+QString::number(privMap[p])+" ("+QString::number((long long )p, 16)+")";
}


StorageEventPrivate::StorageEventPrivate(DataStore & store, const StorageEventType type, QVariantMap data)
	: mStore(store)
	, mType(type)
	, mData(data)
	, mStarted(false)
	, mFinished(false)
	, mSuccessfull(false)
{
	OC_METHODGATE();
	//qDebug()<<"Transaction private created ( store=" << store.filename() <<", type=" << type<<" ) from thread "<<handleCounterString(QThread::currentThreadId())<< " and P="<<privCounterString(this);
}

////////////////////////////////////////////////////////////////////////////////


StorageEvent::StorageEvent(DataStore & store, const StorageEventType type, QVariantMap data)
	: p_ptr(OC_NEW StorageEventPrivate(store, type, data))
{
}



StorageEvent::StorageEvent(StorageEvent && other)
	: p_ptr(other.p_ptr)
{
	//swap(*this, other);

}


StorageEvent::StorageEvent(const StorageEvent & other)
//	: d_ptr(OC_NEW TransactionPrivate(other.d_func()->mStore, other.d_func()->mType, other.d_func()->mData))
	: p_ptr(other.p_ptr)
{

}

StorageEvent::StorageEvent(StorageEventPrivate &pp)
	: p_ptr(&pp)
{
}



StorageEvent & StorageEvent::operator=(StorageEvent other)
{
	//if(!p_ptr.isNull()){	p_ptr.reset();	}
	p_ptr=other.p_ptr;
	return *this;
}

bool StorageEvent::operator==(StorageEvent &other)
{
	return ( other.p_ptr == p_ptr );
}


bool StorageEvent::operator!=(StorageEvent &other)
{
	return !operator==(other);
}

/*
void swap(Transaction& first, Transaction& second)  Q_DECL_NOTHROW
{
	std::swap(first.p_ptr, second.p_ptr);
}
*/


DataStore & StorageEvent::store() const
{
	OC_METHODGATE();
	const StorageEventPrivate *p=p_ptr.data();
	return p->mStore;
}

StorageEventType StorageEvent::type() const
{
	OC_METHODGATE();
	const StorageEventPrivate *p=p_ptr.data();
	return p->mType;
}

QVariantMap StorageEvent::data() const
{
	OC_METHODGATE();
	const StorageEventPrivate *p=p_ptr.data();
	return p->mData;
}

bool StorageEvent::isStarted()
{
	OC_METHODGATE();
	StorageEventPrivate *p=p_ptr.data();
	QMutexLocker ml(&p->mStartedMutex);
	return p->mStarted;
}


bool StorageEvent::isFinished()
{
	OC_METHODGATE();
	StorageEventPrivate *p=p_ptr.data();
	QMutexLocker ml(&p->mFinishedMutex);
	return p->mFinished;
}

void StorageEvent::notifyFinished()
{
	OC_METHODGATE();
	StorageEventPrivate *p=p_ptr.data();
	//qDebug()<<"Entered Transaction::notifyFinished() from thread "<<handleCounterString(QThread::currentThreadId())<< " and P="<<privCounterString(p);
	QMutexLocker finishedLock(&p->mFinishedMutex);
	//const bool oldFinished=p->mFinished;
	p->mFinished=true;
	//qDebug()<<"NOTIFY changed finished from "<<oldFinished << " to " <<p->mFinished;
	// Get values while they are under lock for the return
	// bool ret = p->mFinished && p->mSuccessfull;
	//qDebug().noquote().nospace()<<" + Transaction::run() signalling finished transaction from thread "<<handleCounterString(QThread::currentThreadId());
	//qDebug().noquote().nospace()<<" + Transaction::run() WAKE ALL";
	p->mFinishedCond.wakeAll();
	//qDebug().noquote().nospace()<<" + Transaction::run() ALL AWOKE";
}

void StorageEvent::waitForFinished()
{
	OC_METHODGATE();
	StorageEventPrivate *p=p_ptr.data();
	//qDebug()<<"Entered Transaction::waitForFinished() from thread "<<handleCounterString(QThread::currentThreadId())<< " and P="<<privCounterString(p);
	QMutexLocker ml(&p->mFinishedMutex);

	while(! p->mFinished) {
		//qDebug()<<"BEFORE WAIT, finished was "<<p->mFinished;
		//qDebug()<<" + Transaction::waitForFinished() WAITING from thread "<<handleCounterString(QThread::currentThreadId());
		p->mFinishedCond.wait(&p->mFinishedMutex);
		//qDebug()<<"AFTER WAIT, finished was "<<p->mFinished;
		ml.unlock();
		ml.relock();
		//qDebug()<<" + Transaction::waitForFinished() NOTIFIED from thread "<<handleCounterString(QThread::currentThreadId())<< " with finished= "<<p->mFinished;
	}

	//qDebug()<<"Exiting Transaction::waitForFinished() from thread "<<handleCounterString(QThread::currentThreadId());
}

bool StorageEvent::isSuccessfull()
{
	OC_METHODGATE();
	StorageEventPrivate *p=p_ptr.data();
	QMutexLocker ml(&p->mSuccessfullMutex);
	return p->mSuccessfull;
}

QString StorageEvent::message()
{
	OC_METHODGATE();
	StorageEventPrivate *p=p_ptr.data();
	QMutexLocker ml(&p->mMessageMutex);
	return p->mMessage;
}

bool StorageEvent::run()
{
	OC_METHODGATE();
	//qDebug()<<"Entered Transaction::run() from thread "<<handleCounterString(QThread::currentThreadId());
	StorageEventPrivate *p=p_ptr.data();
	QMutexLocker startedLock(&p->mStartedMutex);
	bool ret=false;
	// We only run once
	if(! p->mStarted) {
		p->mStarted=true;
		QMutexLocker messageLock(&p->mMessageMutex);
		QMutexLocker successfullLock(&p->mSuccessfullMutex);
		startedLock.unlock();

		switch(p->mType) {
		case(STORAGE_EVENT_CLEAR): {
			p->mMessage="clear started";
			bool b=p->mStore.clearSync();
			p->mSuccessfull=b;
			p->mMessage=p->mSuccessfull?"clear succeeded":"clear failed";
		}
		break;
		case(STORAGE_EVENT_GET): {
			p->mMessage="get started";
			p->mData=p->mStore.getSync();
			p->mSuccessfull=true;
			p->mMessage=p->mSuccessfull?"get succeeded":"get failed";
		}
		break;
		case(STORAGE_EVENT_SET): {
			p->mMessage="set started";
			p->mSuccessfull=p->mStore.setSync(p->mData);
			p->mMessage=p->mSuccessfull?"set succeeded":"set failed";
		}
		break;
		case(STORAGE_EVENT_LOAD): {
			p->mMessage="load started";
			p->mSuccessfull=p->mStore.loadSync();
			p->mMessage=p->mSuccessfull?"load succeeded":"load failed";
		}
		break;
		case(STORAGE_EVENT_SAVE): {
			p->mMessage="save started";
			p->mSuccessfull=p->mStore.saveSync();
			p->mMessage=p->mSuccessfull?"save succeeded":"save failed";
		}
		case(STORAGE_EVENT_SYNCHRONIZE): {
			p->mMessage="synchronization started";
			p->mSuccessfull=p->mStore.synchronizeSync();
			p->mMessage=p->mSuccessfull?"synchronization succeeded":"synchronization failed";
		}
		break;
		case(STORAGE_EVENT_DONE): {
			p->mMessage="completion started";
			p->mSuccessfull=p->mStore.completeSync();
			p->mMessage=p->mSuccessfull?"complete succeeded":"complete failed";
		}
		break;
		default: {
			p->mMessage="unknown transaction type";
			p->mSuccessfull=false;
		}
		break;
		}
		notifyFinished();
	} else {
		qWarning()<<"ERROR: Trying to re-run transaction";
	}
	//qDebug().noquote().nospace()<<"Exiting Transaction::run() with return value "<<ret<< " from thread "<<handleCounterString(QThread::currentThreadId());
	return ret;
}



////////////////////////////////////////////////////////////////////////////////





DataStore::DataStore(QString filename, QObject *parent)
	: QObject(parent)
	, mFilename(filename)
	, mAutoIncrement(0)
	, mDiskCounter(0)
	, mMemoryCounter(0)
	, mDone(false)
{
	OC_METHODGATE();
	setObjectName("DataStore");
	QFile file(mFilename);
	const bool exists=file.exists();
	qDebug().noquote().nospace()<<"DataStore created () with filename="<< mFilename <<" (exists="<<exists<<") from thread "<<handleCounterString(QThread::currentThreadId());
	if(exists) {
		// Make sure that a sync from initial state will perform a load if there is data on disk
		mDiskCounter=autoIncrement();
	}
	// Start transaction processing in separate thread
	mCompleteFuture=QtConcurrent::run([this]() {
		OC_METHODGATE();
		//qDebug()<<"Entered DataStore::QtConcurrent::run::lambda() from thread "<<handleCounterString(QThread::currentThreadId());
		processTransactions();
		//qDebug()<<"Exiting DataStore::QtConcurrent::run::lambda() from thread "<<handleCounterString(QThread::currentThreadId());
	});
}

DataStore::~DataStore()
{
	OC_METHODGATE();
	//qDebug()<<"Entered DataStore::~DataStore() from thread "<<handleCounterString(QThread::currentThreadId());
	// TODO: Look at how to avoid the possibility of an unecessary load during this final sync
	synchronize();
	complete();
	mCompleteFuture.waitForFinished();
	//qDebug()<<"Exiting DataStore::~DataStore() from thread "<<handleCounterString(QThread::currentThreadId());
}




QString DataStore::filename() const
{
	OC_METHODGATE();
	return mFilename;
}

bool DataStore::fileExists() const
{
	OC_METHODGATE();
	return QFile(mFilename).exists();
}

bool DataStore::ready()
{
	OC_METHODGATE();
	return mMemoryCounter > 0;
}



StorageEvent DataStore::enqueueTransaction(StorageEvent trans)
{
	OC_METHODGATE();
	//qDebug()<<"Entered DataStore::enqueueTransaction() from thread "<<handleCounterString(QThread::currentThreadId());
	mTransactions.put(trans);
	//qDebug()<<"Exiting DataStore::enqueueTransaction() from thread "<<handleCounterString(QThread::currentThreadId());
	return trans;
}


void DataStore::processTransactions()
{
	OC_METHODGATE();
	//QThread::msleep(2000);
	//qDebug()<<"Entered DataStore::processTransactions() from thread "<<handleCounterString(QThread::currentThreadId());
	while(!mDone) {
		StorageEvent trans=mTransactions.get();
		//qDebug()<<" + DataStore::processTransactions() running transaction with type="<<trans.type()<<"	from thread "<<handleCounterString(QThread::currentThreadId());
		trans.run();
		//qDebug()<<" + DataStore::processTransactions() done running transaction with type="<<trans.type()<<" from thread "<<handleCounterString(QThread::currentThreadId());
	}
	//qDebug()<<"Exiting DataStore::processTransactions() from thread "<<handleCounterString(QThread::currentThreadId());
}

quint64 DataStore::autoIncrement()
{
	OC_METHODGATE();
	return ++mAutoIncrement;
}



StorageEvent DataStore::clear()
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_CLEAR));
}

StorageEvent DataStore::get()
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_GET));
}

StorageEvent DataStore::set(QVariantMap data)
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_SET, data));
}

StorageEvent DataStore::load()
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_LOAD));
}

StorageEvent DataStore::save()
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_SAVE));
}

StorageEvent DataStore::synchronize()
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_SYNCHRONIZE));
}


StorageEvent DataStore::complete()
{
	OC_METHODGATE();
	return enqueueTransaction(StorageEvent(*this, STORAGE_EVENT_DONE));
}


bool DataStore::clearSync()
{
	OC_METHODGATE();
	bool succeeded=false;
	QFile file(mFilename);
	bool fileOk=false;
	if(file.exists()) {
		if(file.remove()) {
			fileOk=true;
		} else {
			qWarning().noquote().nospace()<<"ERROR: Could not clear file '"<<mFilename<<"'";
		}
	} else {
		fileOk=true;
	}
	if(fileOk) {
		mDiskCounter=0;
		mData.clear();
		const auto old=mMemoryCounter;
		mMemoryCounter=0;
		//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA CLEAR";
		succeeded=true;
	}
	return succeeded;
}




QVariantMap DataStore::getSync()
{
	OC_METHODGATE();
	//qDebug()<<"Exiting Sync Get with data="<<mData;
	return mData;
}


bool DataStore::setSync(QVariantMap data)
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Set with data="<<data;
	const bool succeeded=true;
	mData=data;
	const auto old=mMemoryCounter;
	mMemoryCounter=autoIncrement();
	//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA AUTOINCREMENT";
	//qDebug()<<"Exiting Sync Set with succeeded="<<succeeded;
	return succeeded;
}


bool DataStore::loadSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Load";
	bool succeeded=false;
	qDebug().noquote().nospace()<<"Loading datastore from file '"<<mFilename<<"'";
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(mFilename);
	if(raw.size()<=0) {
		//Let empty data pass because it is valid the first time app starts
		qWarning().nospace().noquote() << "WARNING: Data read from file '"<<mFilename<< "' was empty";
		mData=QVariantMap();
	} else {
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning().nospace().noquote() << "ERROR: Parsing json data "<<raw<<" from file '"<<mFilename<<"' produced error '"<<jsonError.errorString()<<"'";
			succeeded=false;
		} else {
			mData=doc.object().toVariantMap();
			const auto old=mMemoryCounter;
			mMemoryCounter=mDiskCounter;
			//qDebug()<<"MEMORY COUNTER FOR "<<mFilename<<" WENT FROM "<<old<<" to " <<mMemoryCounter<< " VIA DISK COUNTER";
			succeeded=true;
		}
	}
	//qDebug()<<"Exiting Sync Load with succeeded="<<succeeded;
	return succeeded;
}

bool DataStore::saveSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Save";
	bool succeeded=false;
	qDebug().noquote().nospace()<<"Saving datastore to file '"<<mFilename<<"'";
	const auto t=QDateTime::currentDateTime();
	if(!mData.contains("createdTimeStamp")) {
		mData["createdTimeStamp"]=t;
	}
	mData["modifiedTimeStamp"]=t;
	QJsonDocument doc=QJsonDocument::fromVariant(mData);
	succeeded=utility::stringToFile(mFilename, doc.toJson());
	if(succeeded) {
		mDiskCounter=mMemoryCounter;
	}
	//qDebug()<<"Exiting Sync Save with succeeded="<<succeeded;
	return succeeded;
}


bool DataStore::synchronizeSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Synchronize";
	bool succeeded=false;
	const quint64 disk=mDiskCounter;
	const quint64 mem=mMemoryCounter;
	if(disk == mem) {
		succeeded=true;
	} else if(disk > mem) {
		succeeded=loadSync();
	} else if(disk < mem) {
		succeeded=saveSync();
	}
	//qDebug()<<"Exiting Sync Synchronize with succeeded="<<succeeded;
	return succeeded;
}


bool DataStore::completeSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Completion";
	bool succeeded=true;
	mDone=true;
	//qDebug()<<"Exiting Sync Completion with succeeded="<<succeeded;
	return succeeded;
}



const QDebug &operator<<(QDebug &d, DataStore &ks)
{
	OC_FUNCTIONGATE();
	d.nospace() <<"DataStore{ fn="<<ks.mFilename;
//	d.nospace() <<", fexists="<<ks.fileExists()<<", ready="<<(const bool)ks.mLoadCompleted<<", inProgress="<<(const bool)ks.mLoadInProgress<<", error="<<(const bool)ks.mLoadHadErrors<<", config:{";
	d.nospace() << ks.mData;
	d.nospace() <<"}}";
	return d.maybeSpace();
}



////////////////////////////////////////////////////////////////////////////////




SimpleDataStore::SimpleDataStore(QString filename)
	: mDataStore(filename)
{
	OC_METHODGATE();
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



////////////////////////////////////////////////////////////////////////////////




SimpleMapStore::SimpleMapStore(QString filename)
	: SimpleDataStore(filename)
{
	OC_METHODGATE();
}

SimpleMapStore::~SimpleMapStore()
{
	OC_METHODGATE();
}

bool SimpleMapStore::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	mMap=data;
	return true;
}

QVariantMap SimpleMapStore::toMap()
{
	OC_METHODGATE();
	return mMap;
}
