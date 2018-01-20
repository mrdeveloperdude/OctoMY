#include "DataStore.hpp"


#include "pose/PoseMapping.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QJsonParseError>
#include <QByteArray>


TransactionPrivate::TransactionPrivate(DataStore & store, const TransactionType type, QVariantMap data)
	: mStore(store)
	, mType(type)
	, mData(data)
	, mStarted(false)
	, mFinished(false)
	, mSuccessfull(false)
{
	OC_METHODGATE();
}

////////////////////////////////////////////////////////////////////////////////

Transaction::Transaction(const Transaction &other)
	: d_ptr(OC_NEW TransactionPrivate(other.d_func()->mStore, other.d_func()->mType, other.d_func()->mData))
{

}


Transaction::Transaction(DataStore & store, const TransactionType type, QVariantMap data)
	: d_ptr(OC_NEW TransactionPrivate(store, type, data))
{
}

Transaction::Transaction(TransactionPrivate &dd)
	: d_ptr(&dd)
{
}


Transaction::Transaction(Transaction && other) : Transaction()
{
	swap(*this, other);
}

Transaction & Transaction::operator=(Transaction other)
{
	swap(*this, other);
	return *this;
}

bool Transaction::operator==(Transaction &other)
{
	return ( other.d_func() == this->d_func()   );
}


bool Transaction::operator!=(Transaction &other)
{
	return !operator==(other);
}

void swap(Transaction& first, Transaction& second) /* nothrow */
{
	using std::swap;
	swap(first.d_ptr, second.d_ptr);
}



DataStore & Transaction::store() const
{
	OC_METHODGATE();
	Q_D(const Transaction);
	return d->mStore;
}

TransactionType Transaction::type() const
{
	OC_METHODGATE();
	Q_D(const Transaction);
	return d->mType;
}

QVariantMap Transaction::data() const
{
	OC_METHODGATE();
	Q_D(const Transaction);
	return d->mData;
}

bool Transaction::isStarted()
{
	OC_METHODGATE();
	Q_D(Transaction);
	QMutexLocker ml(&d->mStartedMutex);
	return d->mStarted;
}


bool Transaction::isFinished()
{
	OC_METHODGATE();
	Q_D(Transaction);
	QMutexLocker ml(&d->mFinishedMutex);
	return d->mFinished;
}


void Transaction::waitForFinished()
{
	OC_METHODGATE();
	qDebug()<<"Entered Transaction::waitForFinished() from thread "<<QThread::currentThreadId();
	Q_D(Transaction);
	QMutexLocker ml(&d->mFinishedMutex);

	while(! d->mFinished) {
		qDebug()<<"NOTE: finished was "<<d->mFinished;
		qDebug()<<" + Transaction::waitForFinished() waiting from thread "<<QThread::currentThreadId();
		d->mFinishedCond.wait(&d->mFinishedMutex);
		ml.unlock();
		ml.relock();
	}

	qDebug()<<"Exiting Transaction::waitForFinished() from thread "<<QThread::currentThreadId();
}



bool Transaction::isSuccessfull()
{
	OC_METHODGATE();
	Q_D(Transaction);
	QMutexLocker ml(&d->mSuccessfullMutex);
	return d->mSuccessfull;
}

QString Transaction::message()
{
	OC_METHODGATE();
	Q_D(Transaction);
	QMutexLocker ml(&d->mMessageMutex);
	return d->mMessage;
}



bool Transaction::run()
{
	OC_METHODGATE();
	qDebug()<<"Entered Transaction::run() from thread "<<QThread::currentThreadId();
	Q_D(Transaction);
	QMutexLocker startedLock(&d->mStartedMutex);
	bool ret=false;
	// We only run once
	if(! d->mStarted) {
		d->mStarted=true;
		QMutexLocker messageLock(&d->mMessageMutex);
		QMutexLocker successfullLock(&d->mSuccessfullMutex);
		startedLock.unlock();

		switch(d->mType) {
		case(TRANSACTION_CLEAR): {
			d->mMessage="clear started";
			bool b=d->mStore.clearSync();
			d->mSuccessfull=b;
			d->mMessage=d->mSuccessfull?"clear succeeded":"clear failed";
		}
		break;
		case(TRANSACTION_GET): {
			d->mMessage="get started";
			d->mData=d->mStore.getSync();
			d->mSuccessfull=true;
			d->mMessage=d->mSuccessfull?"get succeeded":"get failed";
		}
		break;
		case(TRANSACTION_SET): {
			d->mMessage="set started";
			d->mSuccessfull=d->mStore.setSync(d->mData);
			d->mMessage=d->mSuccessfull?"set succeeded":"set failed";
		}
		break;
		case(TRANSACTION_LOAD): {
			d->mMessage="load started";
			d->mSuccessfull=d->mStore.loadSync();
			d->mMessage=d->mSuccessfull?"load succeeded":"load failed";
		}
		break;
		case(TRANSACTION_SAVE): {
			d->mMessage="save started";
			d->mSuccessfull=d->mStore.saveSync();
			d->mMessage=d->mSuccessfull?"save succeeded":"save failed";
		}
		case(TRANSACTION_SYNCHRONIZE): {
			d->mMessage="synchronization started";
			d->mSuccessfull=d->mStore.synchronizeSync();
			d->mMessage=d->mSuccessfull?"synchronization succeeded":"synchronization failed";
		}
		break;
		default: {
			d->mMessage="unknown transaction type";
			d->mSuccessfull=false;
		}
		break;
		}
		{
			QMutexLocker finishedLock(&d->mFinishedMutex);
			qDebug()<<"NOTE BEFORE: finished was "<<d->mFinished;
			d->mFinished=true;
			// Get values while they are under lock for the return
			ret = d->mFinished && d->mSuccessfull;
			qDebug()<<"NOTE AFTER: finished was "<<d->mFinished;
			qDebug().noquote().nospace()<<" + Transaction::run() signalling finished transaction from thread "<<QThread::currentThreadId();
		}
		qDebug().noquote().nospace()<<" + Transaction::run() WAKE ALLLLLLLLL";
		d->mFinishedCond.wakeAll();
	} else {
		qWarning()<<"ERROR: Trying to re-run transaction";
	}
	qDebug().noquote().nospace()<<"Exiting Transaction::run() with return value "<<ret<< " from thread "<<QThread::currentThreadId();
	return ret;
}



////////////////////////////////////////////////////////////////////////////////





DataStore::DataStore(QString filename, QObject *parent)
	: QObject(parent)
	, mFilename(filename)
	, mThreadStarted(false)
	, mAutoIncrement(0)
	, mDiskCounter(0)
	, mMemoryCounter(0)
	, mDone(false)
{
	OC_METHODGATE();
	setObjectName("DataStore");
	QFile file(mFilename);
	if(file.exists()) {
		// Make sure that a sync from initial state will perform a load if there is data on disk
		mDiskCounter=autoIncrement();
	}
	QtConcurrent::run([this]() {
		OC_METHODGATE();
		qDebug()<<"Entered DataStore::QtConcurrent::run::lambda() from thread "<<QThread::currentThreadId();
		processTransactions();
		qDebug()<<"Exiting DataStore::QtConcurrent::run::lambda() from thread "<<QThread::currentThreadId();
	});


	QMutexLocker ml(&mThreadStartedMutex);
	if (!mThreadStarted) {
		mThreadStartedWait.wait(&mThreadStartedMutex);
	}

}

DataStore::~DataStore()
{
	OC_METHODGATE();
	qDebug()<<"Entered DataStore::~DataStore() from thread "<<QThread::currentThreadId();
	mDone=true;
	// TODO: Look at how to avoid the possibility of an unecessary load during this final sync
	synchronize().waitForFinished();
	qDebug()<<"Exiting DataStore::~DataStore() from thread "<<QThread::currentThreadId();
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



Transaction DataStore::enqueueTransaction(Transaction trans)
{
	OC_METHODGATE();
	qDebug()<<"Entered DataStore::enqueueTransaction() from thread "<<QThread::currentThreadId();
	{
		QMutexLocker ml(&mTransactionLogMutex);
		const int count=mTransactionLog.count();
		if (count >= 100) {
			qDebug()<<" + DataStore::enqueueTransaction() waiting for not full";
			mTransactionLogNotFull.wait(&mTransactionLogMutex);
			qDebug()<<" + DataStore::enqueueTransaction() got not full";
		}
		mTransactionLog.push_back(trans);
		qDebug()<<" + DataStore::enqueueTransaction() appended transaction giving total of "<<mTransactionLog.count();
	}
	{
		QMutexLocker ml(&mTransactionLogMutex);
		mTransactionLogNotEmpty.wakeAll();
	}
	qDebug()<<"Exiting DataStore::enqueueTransaction() from thread "<<QThread::currentThreadId();
	return trans;
}


void DataStore::processTransactions()
{
	OC_METHODGATE();
	qDebug()<<"Entered DataStore::processTransactions() from thread "<<QThread::currentThreadId();
	{
		QMutexLocker ml(&mThreadStartedMutex);
		mThreadStarted=true;
		mThreadStartedWait.wakeAll();
	}
	while(!mDone) {
		{
			QMutexLocker ml(&mTransactionLogMutex);
			const int count=mTransactionLog.count();
			if (count <=0) {
				qDebug()<<" + DataStore::processTransactions() waiting for not empty from thread "<<QThread::currentThreadId();
				mTransactionLogNotEmpty.wait(&mTransactionLogMutex);
				qDebug()<<" + DataStore::processTransactions() got not empty: "<<mTransactionLog.count()<< " from thread "<<QThread::currentThreadId();
			}
		}
		Transaction trans;
		{
			QMutexLocker ml(&mTransactionLogMutex);
			trans=mTransactionLog.takeFirst();
			qDebug()<<" + DataStore::processTransactions() fetched transaction leaving behind "<<mTransactionLog.count();
			mTransactionLogNotFull.wakeAll();
		}
		qDebug()<<" + DataStore::processTransactions() running transaction";
		trans.run();
		qDebug()<<" + DataStore::processTransactions() done running transaction";
	}
	qDebug()<<"Exiting DataStore::processTransactions() from thread "<<QThread::currentThreadId();
}

quint64 DataStore::autoIncrement()
{
	OC_METHODGATE();
	return mAutoIncrement++;
}



Transaction DataStore::clear()
{
	OC_METHODGATE();
	return enqueueTransaction(Transaction(*this, TRANSACTION_CLEAR));
}

Transaction DataStore::get()
{
	OC_METHODGATE();
	return enqueueTransaction(Transaction(*this, TRANSACTION_GET));
}

Transaction DataStore::set(QVariantMap data)
{
	OC_METHODGATE();
	return enqueueTransaction(Transaction(*this, TRANSACTION_SET, data));
}


Transaction DataStore::load()
{
	OC_METHODGATE();
	return enqueueTransaction(Transaction(*this, TRANSACTION_LOAD));
}


Transaction DataStore::save()
{
	OC_METHODGATE();
	return enqueueTransaction(Transaction(*this, TRANSACTION_SAVE));
}


Transaction DataStore::synchronize()
{
	OC_METHODGATE();
	return enqueueTransaction(Transaction(*this, TRANSACTION_SYNCHRONIZE));
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
		mMemoryCounter=0;
		succeeded=true;
	}
	return succeeded;
}




QVariantMap DataStore::getSync()
{
	OC_METHODGATE();
	return mData;
}


bool DataStore::setSync(QVariantMap data)
{
	OC_METHODGATE();
	mData=data;
	mMemoryCounter=autoIncrement();
	return true;
}


bool DataStore::loadSync()
{
	OC_METHODGATE();
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
			mMemoryCounter=mDiskCounter;
			succeeded=true;
		}
	}
	return succeeded;
}

bool DataStore::saveSync()
{
	OC_METHODGATE();
	bool succeeded=false;
	qDebug().noquote().nospace()<<"Saving datastore to file '"<<mFilename<<"'";
	auto t=QDateTime::currentMSecsSinceEpoch();
	if(!mData.contains("createdTimeStamp")) {
		mData["createdTimeStamp"]=t;
	}
	mData["modifiedTimeStamp"]=t;
	QJsonDocument doc=QJsonDocument::fromVariant(mData);
	succeeded=utility::stringToFile(mFilename, doc.toJson());
	if(succeeded) {
		mDiskCounter=mMemoryCounter;
	}

	return succeeded;
}


bool DataStore::synchronizeSync()
{
	OC_METHODGATE();
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
}

SimpleMapStore::~SimpleMapStore()
{

}

bool SimpleMapStore::fromMap(QVariantMap data)
{
	mMap=data;
	return true;
}

QVariantMap SimpleMapStore::toMap()
{
	return mMap;
}
