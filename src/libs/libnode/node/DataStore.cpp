#include "DataStore.hpp"

#ifdef USE_OLD_STORE

#include "pose/PoseMapping.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QJsonParseError>
#include <QByteArray>
#include <QMap>



DataStore::DataStore(QString filename, QObject *parent)
	//: QObject(parent)
	: mFilename(filename)
	, mAutoIncrement(0)
	, mDiskCounter(0)
	, mMemoryCounter(0)
	, mDone(false)
{
	OC_METHODGATE();
//	setObjectName("DataStore");
	QFile file(mFilename);
	const bool exists=file.exists();
	//qDebug().noquote().nospace()<<"DataStore created () with filename="<< mFilename <<" (exists="<<exists<<") from thread "<<utility::currentThreadID();
	if(exists) {
		// Make sure that a sync from initial state will perform a load if there is data on disk
		mDiskCounter=autoIncrement();
	}
	// Start transaction processing in separate thread
	mCompleteFuture=QtConcurrent::run([this]() {
		OC_METHODGATE();
		//qDebug()<<"Entered DataStore::QtConcurrent::run::lambda() from thread "<<utility::currentThreadID();
		processTransactions();
		//qDebug()<<"Exiting DataStore::QtConcurrent::run::lambda() from thread "<<utility::currentThreadID();
	});
}

DataStore::~DataStore()
{
	OC_METHODGATE();
	//qDebug()<<"Entered DataStore::~DataStore() from thread "<<utility::currentThreadID();
	// TODO: Look at how to avoid the possibility of an unecessary load during this final sync
	synchronize();
	complete();
	mCompleteFuture.waitForFinished();
	//qDebug()<<"Exiting DataStore::~DataStore() from thread "<<utility::currentThreadID();
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
	//qDebug()<<"Entered DataStore::enqueueTransaction() from thread "<<utility::currentThreadID();
	mTransactions.put(trans);
	//qDebug()<<"Exiting DataStore::enqueueTransaction() from thread "<<utility::currentThreadID();
	return trans;
}


void DataStore::processTransactions()
{
	OC_METHODGATE();
	//QThread::msleep(2000);
	//qDebug()<<"Entered DataStore::processTransactions() from thread "<<utility::currentThreadID();
	while(!mDone) {
		StorageEvent trans=mTransactions.get();
		//qDebug()<<" + DataStore::processTransactions() running transaction with type="<<trans.type()<<"	from thread "<<utility::currentThreadID();
		trans.run();
		//qDebug()<<" + DataStore::processTransactions() done running transaction with type="<<trans.type()<<" from thread "<<utility::currentThreadID();
	}
	//qDebug()<<"Exiting DataStore::processTransactions() from thread "<<utility::currentThreadID();
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
		//const auto old=mMemoryCounter;
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
	//const auto old=mMemoryCounter;
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
	//qDebug().noquote().nospace()<<"Loading datastore from file '"<<mFilename<<"'";
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
			//const auto old=mMemoryCounter;
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


// Bootstrap data into memory
bool DataStore::bootstrapSync()
{
	OC_METHODGATE();
	return true;
}

bool DataStore::synchronizeSync()
{
	OC_METHODGATE();
	//qDebug()<<"Entering Sync Synchronize";
	bool succeeded=false;
	const quint64 disk=mDiskCounter;
	const quint64 mem=mMemoryCounter;
	if(disk == mem) {
		//qDebug()<<"DataStore::synchronizeSync("<<filename()<<"): disk("<<disk<<") == mem("<<mem<<"), no-op";
		if(0 == disk){
			//qDebug()<<" + need to generate";
			succeeded=bootstrapSync();
		}
		else{
			succeeded=true;
		}
	} else if(disk > mem) {
		//qDebug()<<"DataStore::synchronizeSync("<<filename()<<"): disk("<<disk<<") > mem("<<mem<<"), loading";
		succeeded=loadSync();
	} else if(disk < mem) {
		//qDebug()<<"DataStore::synchronizeSync("<<filename()<<"): disk("<<disk<<") < mem("<<mem<<"), saving";
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
	d.nospace() <<"}";
	return d.maybeSpace();
}

#endif
