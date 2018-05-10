#include "StorageEvent.hpp"

#include "utility/Standard.hpp"
#include "node/DataStore.hpp"


////////////////////////////////////////////////////////////////////////////////





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
		d.nospace() << "STORAGE_EVENT_UNKNOWN";
	}
	break;
#undef TransactionType_case_stanza
	}

	return d.space();
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

