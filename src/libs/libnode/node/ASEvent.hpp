#ifndef ASEVENT_HPP
#define ASEVENT_HPP

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "utility/TaskQueue.hpp"


#include "ASEventType.hpp"
#include "AsyncStoreStatus.hpp"

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

	Q_DISABLE_COPY(ASEventPrivate)

private:
	AsyncStore<T> &mStore;

	const ASEventType mType;
	T mData;
	QMutex mStartedMutex;
	QMutex mFinishedMutex;
	QWaitCondition mFinishedCond;
	QMutex mSuccessfullMutex;
	QMutex mMessageMutex;
	QMutex mCallbackMutex;
	bool mStarted;
	bool mFinished;
	bool mSuccessfull;
	AsyncStoreStatus mStatus;
	QString mMessage;



	//typedef std::function<void(ASEvent<T> &)> CallbackType;
	typedef std::function<void()> CallbackType;
	//QVector< CallbackType > mCallbacks;
	TaskQueue< CallbackType > mCallbacks;

public:
	static QString privCounterString(const ASEventPrivate<T> *p);

public:
	explicit ASEventPrivate(AsyncStore<T> & store, const ASEventType type, T data=T());
	explicit ASEventPrivate();
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
	inline ASEvent() Q_DECL_NOTHROW;
	virtual ~ASEvent() {}
public:
	explicit ASEvent(AsyncStore<T> &store, const ASEventType type, T data=T());
	explicit ASEvent(ASEventPrivate<T> &pp);
	explicit ASEvent(QSharedPointer<ASEventPrivate<T> > pp);



	ASEvent(const ASEvent<T> & other);
	ASEvent(ASEvent<T> && other);

	ASEvent<T> & operator=(ASEvent<T> other);


	//friend void swap(ASEvent<T>& first, ASEvent<T>& second) Q_DECL_NOTHROW;
	bool operator==(const ASEvent<T> &other) const;
	bool operator!=(const ASEvent<T> &other) const;



public:
	bool isNull() const;

	QSharedPointer<ASEventPrivate<T> > p();

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
	AsyncStoreStatus status();
	bool run();

};


template <typename T>
bool operator==(const ASEvent<T> &a, const ASEvent<T> &b)
{
	return a.operator ==(b);
}

template <typename T>
bool operator!=(const ASEvent<T> &a, const ASEvent<T> &b)
{
	return a.operator !=(b);
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


template <typename T>
ASEventPrivate<T>::ASEventPrivate(AsyncStore<T> & store, const ASEventType type, T data)
	: mStore(store)
	, mType(type)
	, mData(data)
	, mCallbackMutex(QMutex::Recursive)
	, mStarted(false)
	, mFinished(false)
	, mSuccessfull(false)
	, mStatus(mStore.statusSync())
{

	OC_METHODGATE();
	qDebug()<<"ASEventPrivate created ( store=" << mStore.filename() << ", type=" << mType << " ) from thread " << utility::currentThreadID() << " and P:" << privCounterString(this);
}


template <typename T>
ASEventPrivate<T>::ASEventPrivate()
{
	OC_METHODGATE();
	qDebug()<<"ASEventPrivate created ( defaults ) from thread " << utility::currentThreadID() << " and P:" << privCounterString(this);
}



////////////////////////////////////////////////////////////////////////////////

template <typename T>
template <typename F>
void ASEvent<T>::onFinished(F callBack)
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();

	if(nullptr==p) {
		qWarning().nospace().noquote()<<"ERROR: No p";
		return;
	}
	//std::function<void(ASEvent<T> &)>
	//auto f = [this, callBack](ASEvent<T> &ase) { callBack(ase); };

	//ASEventPrivate::CallbackType
	qDebug().nospace().noquote()<<"P:"<<ASEventPrivate<T>::privCounterString(p) <<" from "<<utility::currentThreadID();
	{
		//qDebug()<<"IS RECURSIVE: "<< p->mCallbackMutex.isRecursive();
		QMutexLocker callbackLock(&p->mCallbackMutex);
		//qDebug().nospace().noquote()<<" onFinished callbakc mutex locked from "<<utility::currentThreadID();
		//auto pcallBack=&callBack;
		p->mCallbacks.put( [this, callBack] { callBack( *this); } );
	}

}


template <typename T>
void ASEvent<T>::runCallbacks()
{
	OC_METHODGATE();
	qDebug()<<"runCallbacks() from "<<utility::currentThreadID();
	ASEventPrivate<T> *p=p_ptr.data();
	if(nullptr==p) {
		qWarning().nospace().noquote()<<"ERROR: No p";
		return;
	}
	qDebug().nospace().noquote()<< " from "<<utility::currentThreadID() <<"P:"<<ASEventPrivate<T>::privCounterString(p);
	//qDebug().nospace().noquote()<<" runCallbacks Locking callbakc mutex from "<<utility::currentThreadID();
	//QVector< std::function<void(ASEvent<T> &)> > copy;
	{
		//qDebug()<<"IS RECURSIVE: " << p->mCallbackMutex.isRecursive();
		//p->mCallbackMutex.unlock();
		//p->mCallbackMutex.unlock();
		//p->mCallbackMutex.unlock();
		QMutexLocker callbackLock(&p->mCallbackMutex);
		//qDebug().nospace().noquote()<<"runCallbacks callbakc mutex locked from "<<utility::currentThreadID();
		//copy=p->mCallbacks;

		p->mCallbacks.runAll();
		/*
		const int nm=p->mCallbacks.size();
		int n=1;
		for(auto callBack: copy) {
			qDebug().nospace().noquote()<<" + ASEVENT CALLBACK: "<<n<< "/" << nm;
			n++;
			callBack(*this);
		}
		*/
		qDebug().nospace().noquote()<<"RUNNING CALLBACKS FOR "<<type()<<" event with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished;
	}

	//qDebug().nospace().noquote()<<" runCallbacks callbakc mutex UNLOCKED from "<<utility::currentThreadID();
}


////////////////////////////////////////////////////////////////////////////////

template <typename T>
ASEvent<T>::ASEvent() Q_DECL_NOTHROW
	: p_ptr(nullptr)
{

}

template <typename T>
ASEvent<T>::ASEvent(AsyncStore<T> & store, const ASEventType type, T data)
	: p_ptr(OC_NEW ASEventPrivate<T>(store, type, data))
{
}


template <typename T>
ASEvent<T>::ASEvent(ASEvent<T> && other)
	: p_ptr(other.p_ptr)
{
	//swap(*this, other);

}

template <typename T>
ASEvent<T>::ASEvent(const ASEvent<T> & other)
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
ASEvent<T>::ASEvent(QSharedPointer<ASEventPrivate<T> > pp)
	: p_ptr(pp)
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
bool ASEvent<T>::operator==(const ASEvent &other) const
{
	return ( other.p_ptr == p_ptr );
}

template <typename T>
bool ASEvent<T>::operator!=(const ASEvent &other) const
{
	return !operator==(other);
}

template <typename T>
bool ASEvent<T>::isNull() const
{
	OC_METHODGATE();
	return (p_ptr.isNull());
}


template <typename T>
QSharedPointer<ASEventPrivate<T> > ASEvent<T>::p()
{
	OC_METHODGATE();
	return p_ptr;
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
AsyncStoreStatus ASEvent<T>::status()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	return p->mStatus;
}


template <typename T>
bool ASEvent<T>::run()
{
	OC_METHODGATE();
	//qDebug()<<"Entered Event::run() from thread "<<utility::currentThreadID();
	ASEventPrivate<T> *p=p_ptr.data();
	if(nullptr == p) {
		qWarning().nospace().noquote()<<"ERROR: No p";
		return false;
	}
	QMutexLocker startedLock(&p->mStartedMutex);
	bool ret=false;
	// We only run once
	if(! p->mStarted) {
		p->mStarted=true;
		QMutexLocker messageLock(&p->mMessageMutex);
		QMutexLocker successfullLock(&p->mSuccessfullMutex);
		startedLock.unlock();
		switch(p->mType) {
		case(AS_EVENT_STATUS): {
			p->mMessage="status started";
			p->mStatus=p->mStore.statusSync();
			p->mSuccessfull=true;
			p->mMessage=p->mSuccessfull?"status succeeded":"status failed";
		}
		break;
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
		notifyFinished();
		qDebug()<<"ASEvent runnibng callbaks with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished<<" P:"<<ASEventPrivate<T>::privCounterString(p);
		p->mStore.runCallbacksForEvent(*this);
		qDebug()<<"ASEvent finished with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished<<" P:"<<ASEventPrivate<T>::privCounterString(p);
	} else {
		qWarning()<<"ERROR: Trying to re-run transaction";
	}
	//qDebug().noquote().nospace()<<"Exiting Event::run() with return value "<<ret<< " from thread "<<utility::currentThreadID();
	return ret;
}


#endif // ASEVENT_HPP
