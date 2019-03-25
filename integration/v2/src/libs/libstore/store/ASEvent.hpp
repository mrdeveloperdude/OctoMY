#ifndef ASEVENT_HPP
#define ASEVENT_HPP

#include "ASEventType.hpp"
#include "AsyncStoreStatus.hpp"


#include "utility/concurrent/TaskQueue.hpp"
#include "utility/concurrent/Concurrent.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

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

	//QMutex mStartedMutex;
	QMutex mFinishedMutex;
	QWaitCondition mFinishedCond;
	//QMutex mSuccessfullMutex;
	//QMutex mMessageMutex;
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
	virtual ~ASEventPrivate();


	QString toString() const;
	operator QString() const;
	operator QString();

public:

	friend class ASEvent<T>;

};




////////////////////////////////////////////////////////////////////////////////



template <typename T>
class ASEvent
{
private:
	QSharedPointer<ASEventPrivate<T> >  p_ptr;

	QString mT;
	QString mAllocation="Unallocated";

public:
	virtual ~ASEvent();
public:


	inline ASEvent() Q_DECL_NOTHROW;


	explicit ASEvent(AsyncStore<T> &store, const ASEventType type, T data=T());
	explicit ASEvent(ASEventPrivate<T> &pp);
	explicit ASEvent(QSharedPointer<ASEventPrivate<T> > pp);



	ASEvent(const ASEvent<T> & other);
	ASEvent(ASEvent<T> && other);



	ASEvent<T> & operator=(ASEvent<T> other);


	//friend void swap(ASEvent<T>& first, ASEvent<T>& second) Q_DECL_NOTHROW;
	bool operator==(const ASEvent<T> &other) const;
	bool operator!=(const ASEvent<T> &other) const;


	QString toString() const;
	operator QString() const;
	operator QString();

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

	void tTest() const;
	bool isSuccessfull();
	QString message();
	AsyncStoreStatus status();
	bool run();

};


template <typename T>
bool operator==(const ASEvent<T> &a, const ASEvent<T> &b)
{
	OC_FUNCTIONGATE();
	return a.operator ==(b);
}

template <typename T>
bool operator!=(const ASEvent<T> &a, const ASEvent<T> &b)
{
	OC_FUNCTIONGATE();
	return a.operator !=(b);
}

////////////////////////////////////////////////////////////////////////////////


template <typename T>
QString ASEventPrivate<T>::toString() const
{
	OC_METHODGATE();
	return QString("ASEventPrivate(type='%1', "
				   "data='%2', "
				   "started='%3', "
				   "finished='%4', "
				   "sucessful='%5', "
				   "status='%6', "
				   "message='%7')")
		   .arg(mType)
		   .arg(mData)
		   .arg(mStarted)
		   .arg(mFinished)
		   .arg(mSuccessfull)
		   .arg(mStatus)
		   .arg(mMessage);
}
template <typename T>
ASEventPrivate<T>::operator QString() const
{
	OC_METHODGATE();
	return toString();
}
template <typename T>
ASEventPrivate<T>::operator QString()
{
	OC_METHODGATE();
	return toString();
}


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
	return "P-"+QString::number(privMap[p])+" ("+QString::number(reinterpret_cast<long long >(p), 16)+")";
}


template <typename T>
ASEventPrivate<T>::ASEventPrivate(AsyncStore<T> & store, const ASEventType type, T data)
	: mStore(store)
	, mType(type)
	, mData(data)
//	, mCallbackMutex(QMutex::Recursive)
	, mStarted(false)
	, mFinished(false)
	, mSuccessfull(false)
	, mStatus(mStore.statusSync())
{

	OC_METHODGATE();
	mCallbacks.activate(true);
	//qDebug()<<"ASEventPrivate created ( store=" << mStore.filename() << ", type=" << mType << " ) from thread " << utility::concurrent::currentThreadID() << " and P:" << privCounterString(this);
}


template <typename T>
ASEventPrivate<T>::ASEventPrivate()
	: mType(AS_EVENT_NONE)
	, mStarted(false)
	, mFinished(false)
	, mSuccessfull(false)
	, mStatus(mStore.statusSync())
{
	OC_METHODGATE();
	//qDebug()<<"ASEventPrivate created ( defaults ) from thread " << utility::concurrent::currentThreadID() << " and P:" << privCounterString(this);
}




template <typename T>
ASEventPrivate<T>::~ASEventPrivate()
{
	OC_METHODGATE();
	mCallbacks.activate(false);
}





////////////////////////////////////////////////////////////////////////////////


template <typename T>
QString ASEvent<T>::toString() const
{
	OC_METHODGATE();
	return QString("ASEvent(T='%1'"
				   ", Allocation='%2'"
				   ", %3")
		   .arg(mT)
		   .arg(mAllocation)
		   .arg(nullptr==p_ptr?"p=null": QString("type='%1'"
				   //							   ", data='%?'"
				   ", started='%2'"
				   ", finish='%3'"
				   ", sucessful='%4'"
				   ", status='%5'"
				   ", message='%6'")
				.arg(ASEventTypeToString(p_ptr->mType))
				//	.arg(p_ptr->mData)
				.arg(p_ptr->mStarted?"true":"false")
				.arg(p_ptr->mFinished?"true":"false")
				.arg(p_ptr->mSuccessfull?"true":"false")
				.arg(p_ptr->mStatus)
				.arg(p_ptr->mMessage));
}

template <typename T>
ASEvent<T>::operator QString() const
{
	OC_METHODGATE();
	return toString();
}
template <typename T>
ASEvent<T>::operator QString()
{
	OC_METHODGATE();
	return toString();
}



template <typename T>
template <typename F>
void ASEvent<T>::onFinished(F callBack)
{
	OC_METHODGATE();
	if(p_ptr.isNull()) {
		qWarning().nospace().noquote()<<"ERROR: No p_ptr";
		return;
	}
	ASEventPrivate<T> *p=p_ptr.data();

	if(nullptr == p) {
		qWarning().nospace().noquote()<<"ERROR: No p";
		return;
	}
	//std::function<void(ASEvent<T> &)>
	//auto f = [this, callBack](ASEvent<T> &ase) { 		callBack(ase);  	};

	//ASEventPrivate::CallbackType
	//qDebug().nospace().noquote()<<"P:"<<ASEventPrivate<T>::privCounterString(p) <<" from "<<utility::concurrent::currentThreadID();
	{
		//qDebug()<<"IS RECURSIVE: "<< p->mCallbackMutex.isRecursive();
		QMutexLocker callbackLock(&p->mCallbackMutex);
		//qDebug().nospace().noquote()<<" onFinished callback mutex locked from "<<utility::concurrent::currentThreadID();
		//auto pcallBack=&callBack;
		ASEvent<T> *copy=OC_NEW ASEvent<T>(*this);
		// TODO: Commented out to pass build
		if(nullptr!= copy) {
			p->mCallbacks.put( [copy, callBack] {
				if(nullptr!= copy)
				{
					callBack( *copy );
					delete copy;
				}
			} );
		}
	}

}


template <typename T>
void ASEvent<T>::runCallbacks()
{
	OC_METHODGATE();
	//qDebug()<<"runCallbacks() from "<<utility::concurrent::currentThreadID();
	ASEventPrivate<T> *p=p_ptr.data();
	if(nullptr==p) {
		qWarning().nospace().noquote()<<"ERROR: No p";
		return;
	}
	//qDebug().nospace().noquote()<< " from "<<utility::concurrent::currentThreadID() <<"P:"<<ASEventPrivate<T>::privCounterString(p);
	//qDebug().nospace().noquote()<<" runCallbacks Locking callbakc mutex from "<<utility::concurrent::currentThreadID();
	//QVector< std::function<void(ASEvent<T> &)> > copy;
	{
		//qDebug()<<"IS RECURSIVE: " << p->mCallbackMutex.isRecursive();
		//p->mCallbackMutex.unlock();
		//p->mCallbackMutex.unlock();
		//p->mCallbackMutex.unlock();
		QMutexLocker callbackLock(&p->mCallbackMutex);
		//qDebug().nospace().noquote()<<"runCallbacks callbakc mutex locked from "<<utility::concurrent::currentThreadID();
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
		//qDebug().nospace().noquote()<<"RUNNING CALLBACKS FOR "<<type()<<" event with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished;
	}

	//qDebug().nospace().noquote()<<" runCallbacks callbakc mutex UNLOCKED from "<<utility::concurrent::currentThreadID();
}


////////////////////////////////////////////////////////////////////////////////



template <typename T>
ASEvent<T>::ASEvent() Q_DECL_NOTHROW
:
p_ptr(nullptr)
, mT(utility::concurrent::currentThreadID())
, mAllocation("default ctor")
{
	OC_METHODGATE();
	//qWarning()<<"ERROR: default ctor called. Should never happen";
	tTest();
}



template <typename T>
ASEvent<T>::ASEvent(AsyncStore<T> & store, const ASEventType type, T data)
	: p_ptr(OC_NEW ASEventPrivate<T>(store, type, data))
	, mT(utility::concurrent::currentThreadID())
	, mAllocation("parameter ctor")
{
	OC_METHODGATE();
	tTest();
}


template <typename T>
ASEvent<T>::ASEvent(ASEvent<T> && other)
	: p_ptr(other.p_ptr)
	, mT(utility::concurrent::currentThreadID())
	, mAllocation("ref move ctor")
{
	OC_METHODGATE();
	tTest();
	//swap(*this, other);
}

template <typename T>
ASEvent<T>::ASEvent(const ASEvent<T> & other)
//	: d_ptr(OC_NEW TransactionPrivate(other.d_func()->mStore, other.d_func()->mType, other.d_func()->mData))
	: p_ptr(other.p_ptr)
	, mT(utility::concurrent::currentThreadID())
	, mAllocation("ref copy ctor")
{
	OC_METHODGATE();
	tTest();
}

template <typename T>
ASEvent<T>::ASEvent(ASEventPrivate<T> &pp)
	: p_ptr(&pp)
	, mT(utility::concurrent::currentThreadID())
	, mAllocation("ref copy private ctor")
{
	OC_METHODGATE();
	tTest();
}

template <typename T>
ASEvent<T>::ASEvent(QSharedPointer<ASEventPrivate<T> > pp)
	: p_ptr(pp)
	, mT(utility::concurrent::currentThreadID())
	, mAllocation("shared ptr copy private ctor")
{
	OC_METHODGATE();
	tTest();
}


template <typename T>
ASEvent<T>::~ASEvent()
{
	OC_METHODGATE();


	mAllocation="Deallocated";
}

template <typename T>
ASEvent<T> & ASEvent<T>::operator=(ASEvent<T> other)
{
	//if(!p_ptr.isNull()){	p_ptr.reset();	}
	p_ptr=other.p_ptr;
	mAllocation=QString("operator= copy");
	tTest();
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
void ASEvent<T>::tTest() const
{
	OC_METHODGATE();
	QString cT=utility::concurrent::currentThreadID();
	//qDebug()<<"CALL THREAD: '"<<cT <<"'"; 	qDebug()<<" RUN THREAD: '"<<mT <<"'";
	//qDebug()<<"ALLOCATION: "<<mAllocation;
	if(mT!=cT) {
		//qDebug()<<"WARNING: THEAD THAT CREATED ASEVENT: '"<<mT<<"' WAS NOT THE ONE THAT CALLED TTEST: '"<<cT <<"'";
		qWarning()<<"ERROR: creator thread not same as executor thread";
	}
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
	tTest();
	const ASEventPrivate<T> *p=p_ptr.data();
	return p->mStore;
}

template <typename T>
ASEventType ASEvent<T>::type() const
{
	OC_METHODGATE();
	const ASEventPrivate<T> *p=p_ptr.data();
	tTest();
	return p->mType;
}


template <typename T>
T ASEvent<T>::data() const
{
	OC_METHODGATE();
	const ASEventPrivate<T> *p=p_ptr.data();
	tTest();
	return p->mData;
}


template <typename T>
bool ASEvent<T>::isStarted()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	tTest();
	//QMutexLocker ml(&p->mStartedMutex);
	return p->mStarted;
}


template <typename T>
bool ASEvent<T>::isFinished()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	tTest();
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
	//qDebug().noquote().nospace()<<" + Transaction::run() signalling finished transaction from thread "<<utility::concurrent::currentThreadID();
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
		//qDebug()<<" + Transaction::waitForFinished() WAITING from thread "<<utility::concurrent::currentThreadID();
		p->mFinishedCond.wait(&p->mFinishedMutex);
		//qDebug()<<"AFTER WAIT, finished was "<<p->mFinished;
		ml.unlock();
		ml.relock();
		//qDebug()<<" + Transaction::waitForFinished() NOTIFIED from thread "<<handleCounterString(QThread::currentThreadId())<< " with finished= "<<p->mFinished;
	}
	//qDebug()<<"Exiting Transaction::waitForFinished() from thread "<<utility::concurrent::currentThreadID();
}



template <typename T>
bool ASEvent<T>::isSuccessfull()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	tTest();
	if(nullptr != p) {
//		QMutexLocker ml(&p->mSuccessfullMutex);
		return p->mSuccessfull;
	}
	return false;
}


template <typename T>
QString ASEvent<T>::message()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	QMutexLocker ml(&p->mMessageMutex);
	return (nullptr==p)?"NULLPOINTER":p->mMessage;
}


template <typename T>
AsyncStoreStatus ASEvent<T>::status()
{
	OC_METHODGATE();
	ASEventPrivate<T> *p=p_ptr.data();
	return (nullptr==p)?AsyncStoreStatus(0, 0):p->mStatus;
}


template <typename T>
bool ASEvent<T>::run()
{
	OC_METHODGATE();
	qDebug()<<"Entered Event::run() from thread "<<utility::concurrent::currentThreadID();
	ASEventPrivate<T> *p=p_ptr.data();
	if(nullptr == p) {
		qWarning().nospace().noquote()<<"ERROR: No p";
		return false;
	}
//	QMutexLocker startedLock(&p->mStartedMutex);
	bool ret=false;
	// We only run once
	if(! p->mStarted) {
		p->mStarted=true;
//		QMutexLocker messageLock(&p->mMessageMutex);
//		QMutexLocker successfullLock(&p->mSuccessfullMutex);
//		startedLock.unlock();
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
		case(AS_EVENT_COMPLETE): {
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
		//qDebug()<<"ASEvent runnibng callbaks with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished<<" P:"<<ASEventPrivate<T>::privCounterString(p);
		p->mStore.runCallbacksForEvent(*this);
		//qDebug()<<"ASEvent finished with message="<<p->mMessage<<" started="<<p->mStarted<<" finished="<<p->mFinished<<" P:"<<ASEventPrivate<T>::privCounterString(p);
	} else {
		qWarning()<<"ERROR: Trying to re-run transaction";
	}
	//qDebug().noquote().nospace()<<"Exiting Event::run() with return value "<<ret<< " from thread "<<utility::concurrent::currentThreadID();
	return ret;
}



#endif // ASEVENT_HPP

