#ifndef ASEVENT_HPP
#define ASEVENT_HPP

#include "utility/Standard.hpp"
//#include "node/DataStoreCallback.hpp"

#include "ASEventType.hpp"

#include <QSharedDataPointer>
#include <QWaitCondition>
#include <QVariantMap>
#include <QMutex>
#include <QVector>


#include <functional>


/**
  Private class for event handle
  ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
*/



template <typename T>
class AsyncBackend;

template <typename T>
class AsyncFrontend;

template <typename T>
class AsyncStore;

template <typename T>
class ASEvent;

template <typename T>
class ASEventPrivate
{

private:
	static quint64 privCounter;
	static QMap<const ASEventPrivate<T> *, quint64> privMap;
	static QString privCounterString(const ASEventPrivate<T> *p);

private:
	AsyncStore<T> &mStore;

	const ASEventType mType;
	T mData;
	QMutex mStartedMutex;
	QMutex mFinishedMutex;
	QWaitCondition mFinishedCond;
	QMutex mSuccessfullMutex;
	QMutex mMessageMutex;
	bool mStarted;
	bool mFinished;
	bool mSuccessfull;
	QString mMessage;

	typedef std::function<void(ASEvent<T> &)> CallbackType;
	QVector< CallbackType > mCallbacks;


public:
	explicit ASEventPrivate(AsyncStore<T> & store, const ASEventType type, T data=T());
	virtual ~ASEventPrivate() {}

public:

	friend class ASEvent<T>;
};




////////////////////////////////////////////////////////////////////////////////



template <typename T>
class ASEvent
{
private:
	QSharedPointer<ASEventPrivate<T> >  p_ptr;

public:
	inline ASEvent() Q_DECL_NOTHROW{}
	virtual ~ASEvent() {}
public:
	explicit ASEvent(AsyncStore<T> &store, const ASEventType type, T data=T());
	explicit ASEvent(ASEventPrivate<T> &pp);

	ASEvent(const ASEvent<T> & other);
	ASEvent(ASEvent<T> && other);

	ASEvent<T> & operator=(ASEvent<T> other);
	bool operator==(ASEvent<T> &other);
	bool operator!=(ASEvent<T> &other);

	//friend void swap(ASEvent<T>& first, ASEvent<T>& second) Q_DECL_NOTHROW;


public:
	AsyncStore<T> & store() const ;
	ASEventType type() const ;
	T data() const ;
	bool isStarted();

	bool isFinished();
	void notifyFinished();
	void waitForFinished();
	template <typename F>
	void onFinished(F callBack);
	void runCallbacks();

	bool isSuccessfull();
	QString message();
	bool run();



};


template <typename T>
template <typename F>
void ASEvent<T>::onFinished(F callBack)
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();

	//std::function<void(ASEvent<T> &)>
	//auto f = [this, callBack](ASEvent<T> &ase) { callBack(ase); };

	//ASEventPrivate::CallbackType
	p->mCallbacks.push_back( callBack );


}


template <typename T>
void ASEvent<T>::runCallbacks()
{
	OC_METHODGATE();
	const ASEventPrivate<T> *p=p_ptr.data();
	int n=0;
	qDebug().nospace().noquote()<<"RUNNING CALLBACKS FOR "<<type()<<" event with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished;
	for(auto callBack: p->mCallbacks) {
		qDebug().nospace().noquote()<<"RUNNING ASEVENT CALLBACK: "<<n;
		n++;
		callBack(*this);
	}
}


////////////////////////////////////////////////////////////////////////////////


template <typename T>
quint64 ASEventPrivate<T>::privCounter=0;

template <typename T>
QMap<const ASEventPrivate<T> *, quint64> ASEventPrivate<T>::privMap;

template <typename T>
QString ASEventPrivate<T>::privCounterString(const ASEventPrivate<T> *p)
{
	OC_FUNCTIONGATE();
	if(nullptr==p) {
		return "P-null";
	}
	if(!privMap.contains(p)) {
		privCounter++;
		privMap.insert(p, privCounter);
	}
	return "P-"+QString::number(privMap[p])+" ("+QString::number((long long )p, 16)+")";
}

/*

static quint64 privCounter=0;

template <typename T>
static QMap<const DSEventPrivate<T> *, quint64> privMap;

template <typename T>
static QString privCounterString(const DSEventPrivate<T> *p)
{
	OC_FUNCTIONGATE();
	if(nullptr==p) {
		return "P-null";
	}
	if(!privMap.contains(p)) {
		privCounter++;
		privMap.insert(p, privCounter);
	}
	return "P-"+QString::number(privMap[p])+" ("+QString::number((long long )p, 16)+")";
}
*/
template <typename T>
ASEventPrivate<T>::ASEventPrivate(AsyncStore<T> & store, const ASEventType type, T data)
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





template <typename T>
ASEvent<T>::ASEvent(AsyncStore<T> & store, const ASEventType type, T data)
	: p_ptr(OC_NEW ASEventPrivate<T>(store, type, data))
{
}


template <typename T>
ASEvent<T>::ASEvent(ASEvent && other)
	: p_ptr(other.p_ptr)
{
	//swap(*this, other);

}

template <typename T>
ASEvent<T>::ASEvent(const ASEvent & other)
//	: d_ptr(OC_NEW TransactionPrivate(other.d_func()->mStore, other.d_func()->mType, other.d_func()->mData))
	: p_ptr(other.p_ptr)
{

}

template <typename T>
ASEvent<T>::ASEvent(ASEventPrivate<T> &pp)
	: p_ptr(&pp)
{
}


template <typename T>
ASEvent<T> & ASEvent<T>::operator=(ASEvent<T> other)
{
	//if(!p_ptr.isNull()){	p_ptr.reset();	}
	p_ptr=other.p_ptr;
	return *this;
}

template <typename T>
bool ASEvent<T>::operator==(ASEvent &other)
{
	return ( other.p_ptr == p_ptr );
}

template <typename T>
bool ASEvent<T>::operator!=(ASEvent &other)
{
	return !operator==(other);
}


template <typename T>
AsyncStore<T> & ASEvent<T>::store() const
{
	OC_METHODGATE();
	const ASEventPrivate<T> *p=p_ptr.data();
	return p->mStore;
}

template <typename T>
ASEventType ASEvent<T>::type() const
{
	OC_METHODGATE();
	const ASEventPrivate<T> *p=p_ptr.data();
	return p->mType;
}


template <typename T>
T ASEvent<T>::data() const
{
	OC_METHODGATE();
	const ASEventPrivate<T> *p=p_ptr.data();
	return p->mData;
}


template <typename T>
bool ASEvent<T>::isStarted()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	QMutexLocker ml(&p->mStartedMutex);
	return p->mStarted;
}


template <typename T>
bool ASEvent<T>::isFinished()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	QMutexLocker ml(&p->mFinishedMutex);
	return p->mFinished;
}


template <typename T>
void ASEvent<T>::notifyFinished()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	//qDebug()<<"Entered Transaction::notifyFinished() from thread "<<handleCounterString(QThread::currentThreadId())<< " and P="<<privCounterString(p);
	QMutexLocker finishedLock(&p->mFinishedMutex);
	//const bool oldFinished=p->mFinished;
	p->mFinished=true;
	//qDebug()<<"NOTIFY changed finished from "<<oldFinished << " to " <<p->mFinished;
	// Get values while they are under lock for the return
	// bool ret = p->mFinished && p->mSuccessfull;
	//qDebug().noquote().nospace()<<" + Transaction::run() signalling finished transaction from thread "<<utility::currentThreadID();
	//qDebug().noquote().nospace()<<" + Transaction::run() WAKE ALL";
	p->mFinishedCond.wakeAll();
	//qDebug().noquote().nospace()<<" + Transaction::run() ALL AWOKE";
}


template <typename T>
void ASEvent<T>::waitForFinished()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	//qDebug()<<"Entered Transaction::waitForFinished() from thread "<<handleCounterString(QThread::currentThreadId())<< " and P="<<privCounterString(p);
	QMutexLocker ml(&p->mFinishedMutex);

	while(! p->mFinished) {
		//qDebug()<<"BEFORE WAIT, finished was "<<p->mFinished;
		//qDebug()<<" + Transaction::waitForFinished() WAITING from thread "<<utility::currentThreadID();
		p->mFinishedCond.wait(&p->mFinishedMutex);
		//qDebug()<<"AFTER WAIT, finished was "<<p->mFinished;
		ml.unlock();
		ml.relock();
		//qDebug()<<" + Transaction::waitForFinished() NOTIFIED from thread "<<handleCounterString(QThread::currentThreadId())<< " with finished= "<<p->mFinished;
	}

	//qDebug()<<"Exiting Transaction::waitForFinished() from thread "<<utility::currentThreadID();
}


template <typename T>
bool ASEvent<T>::isSuccessfull()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	//QMutexLocker ml(&p->mSuccessfullMutex);
	return p->mSuccessfull;
}


template <typename T>
QString ASEvent<T>::message()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	QMutexLocker ml(&p->mMessageMutex);
	return p->mMessage;
}

template <typename T>
bool ASEvent<T>::run()
{
	OC_METHODGATE();
	//qDebug()<<"Entered Event::run() from thread "<<utility::currentThreadID();
	ASEventPrivate<T> *p=p_ptr.data();
	QMutexLocker startedLock(&p->mStartedMutex);
	bool ret=false;
	// We only run once
	if(! p->mStarted) {
		p->mStarted=true;
		QMutexLocker messageLock(&p->mMessageMutex);
		QMutexLocker successfullLock(&p->mSuccessfullMutex);
		startedLock.unlock();
		switch(p->mType) {
		case(AS_EVENT_CLEAR): {
			p->mMessage="clear started";
			p->mSuccessfull=p->mStore.clearSync();
			p->mMessage=p->mSuccessfull?"clear succeeded":"clear failed";
		}
		break;
		case(AS_EVENT_GET): {
			p->mMessage="get started";
			p->mSuccessfull=false;;
			p->mData=p->mStore.getSync(p->mSuccessfull);
			p->mMessage=p->mSuccessfull?"get succeeded":"get failed";
		}
		break;
		case(AS_EVENT_SET): {
			p->mMessage="set started";
			p->mSuccessfull=p->mStore.setSync(p->mData);
			p->mMessage=p->mSuccessfull?"set succeeded":"set failed";
		}
		break;
		case(AS_EVENT_LOAD): {
			p->mMessage="load started";
			p->mSuccessfull=p->mStore.loadSync();
			p->mMessage=p->mSuccessfull?"load succeeded":"load failed";
		}
		break;
		case(AS_EVENT_SAVE): {
			p->mMessage="save started";
			p->mSuccessfull=p->mStore.saveSync();
			p->mMessage=p->mSuccessfull?"save succeeded":"save failed";
		}
		break;
		case(AS_EVENT_GENERATE): {
			p->mMessage="generate started";
			p->mSuccessfull=p->mStore.generateSync();
			p->mMessage=p->mSuccessfull?"generate succeeded":"generate failed";
		}
		break;
		case(AS_EVENT_SYNCHRONIZE): {
			p->mMessage="synchronization started";
			p->mSuccessfull=p->mStore.synchronizeSync();
			p->mMessage=p->mSuccessfull?"synchronization succeeded":"synchronization failed";
		}
		break;
		case(AS_EVENT_EXISTS): {
			p->mMessage="exists started";
			p->mSuccessfull=p->mStore.fileExists();
			p->mMessage=p->mSuccessfull?"exists succeeded":"exists failed";
		}
		break;
		case(AS_EVENT_READY): {
			p->mMessage="ready started";
			p->mSuccessfull=p->mStore.ready();
			p->mMessage=p->mSuccessfull?"ready succeeded":"ready failed";
		}
		break;
		case(AS_EVENT_DONE): {
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
		qDebug()<<"ASEvent finished with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished;
		notifyFinished();
		runCallbacks();
	} else {
		qWarning()<<"ERROR: Trying to re-run transaction";
	}
	//qDebug().noquote().nospace()<<"Exiting Event::run() with return value "<<ret<< " from thread "<<utility::currentThreadID();
	return ret;
}


#endif // ASEVENT_HPP
