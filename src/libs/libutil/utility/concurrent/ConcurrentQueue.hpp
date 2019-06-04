#ifndef CONCURRENTQUEUE_HPP
#define CONCURRENTQUEUE_HPP

#include "utility/concurrent/AtomicBoolean.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "app/Constants.hpp"

#include <QMutex>
#include <QWaitCondition>
#include <QLinkedList>
#include <QAtomicInteger>
#include <QDebug>
#include <QString>

class ConcurrentQueueSignalEmitter: public QObject
{
	Q_OBJECT
public:
	ConcurrentQueueSignalEmitter(QObject *parent=nullptr);

signals:
	void contentChanged();
};



template <typename T>
class ConcurrentQueue
{
private:
	const int mCapacity;

	// Concurrency primiteives to synchronize on queue
	QMutex mMutex;
	QWaitCondition mNotEmpty;
	QWaitCondition mNotFull;

	QLinkedList<T> mItems;

	AtomicBoolean mDone;

	ConfigureHelper mConfigureHelper;

public:
	ConcurrentQueueSignalEmitter mSignalEmitter;

public:
	explicit ConcurrentQueue(int capacity=0);
	virtual ~ConcurrentQueue();


public:
	void activate(const bool on);

	void list();

public:
	void put(T item);
	T get();
	T tryGet(bool &got);
	int count();
	bool isEmpty();
	bool isDone();

};



////////////////////////////////////////////////////////////////////////////////


template <typename T>
ConcurrentQueue<T>::ConcurrentQueue(int capacity)
	: mCapacity(capacity)
	, mDone(false)
	, mConfigureHelper("ConcurrentQueue", false, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
{
	OC_METHODGATE();
}


template <typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
	OC_METHODGATE();
	mDone=true;
	mNotFull.wakeAll();
	mNotEmpty.wakeAll();
}


template <typename T>
void ConcurrentQueue<T>::activate(const bool on)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		if(on) {

		} else {
			mDone=true;
			mNotFull.wakeAll();
			mNotEmpty.wakeAll();
			emit mSignalEmitter.contentChanged();
		}
	}
}


template <typename T>
void ConcurrentQueue<T>::list()
{
	OC_METHODGATE();
	//if(mConfigureHelper.isActivatedAsExpected()) {
		QMutexLocker ml(&mMutex);
		qDebug()<<"LISTING "<<mItems.count()<<" ITEMS ";
	//}
}

template <typename T>
void ConcurrentQueue<T>::put(T item)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		if(!mDone) {
			QMutexLocker ml(&mMutex);
			int count=mItems.count();
			if ((mCapacity > 0) && (count >= mCapacity)) {
				//qDebug()<<"Waiting for not-full with count="<<count;
				mNotFull.wait(&mMutex);
				count=mItems.count();
				//qDebug()<<"Got for not-full with count="<<count;
			}
			mItems.push_front(item);
			//qDebug()<<" + DataStore::enqueueTransaction() appended transaction giving total of "<<mTransactionLog.count();
			//qDebug()<<"Signaling not-empty";
			mNotEmpty.wakeAll();
			emit mSignalEmitter.contentChanged();
			//qDebug()<<"'PUT' DONE";
		} else {
			qWarning()<<"WARNING: Trying to put item when done";
		}
	}
	list();
}


// Block until queue actually contains something before fetching the first item
template <typename T>
T ConcurrentQueue<T>::get()
{
	OC_METHODGATE();
	list();
	if(mConfigureHelper.isActivatedAsExpected()) {
		unsigned long timeOutMillis=1000;
		QMutexLocker ml(&mMutex);
		int count=mItems.count();
		//qDebug()<<"Waiting for not-empty with count="<<count;
		while((count<=0) && (!mDone)) {
//			const bool didTimeOut=
			mNotEmpty.wait(&mMutex, timeOutMillis);
			count=mItems.count();
			//qDebug()<<" NOT EMPTY WAIT LOOP: ... done="<<mDone <<" count="<<count<<" didTimeOut="<<didTimeOut<<"("<<timeOutMillis<<" ms)";
		}
		if(mDone) {
			return T();
		}
		//qDebug()<<"Got not-empty with count="<<count;
		const bool wasFull=(count >= mCapacity);
		T item=mItems.takeLast();
		count=mItems.count();
		const bool isFull=(count < mCapacity);
		//qDebug()<<"State mCapacity="<<mCapacity<<", wasFull="<<wasFull<<", isFull="<<isFull<<", count="<<count;
		if((mCapacity>0) && (wasFull) && (!isFull)) {
			//qDebug()<<"Signaling not-full";
			ml.unlock();
			mNotFull.wakeAll();
			ml.relock();
			emit mSignalEmitter.contentChanged();
		}
		//qDebug()<<"'GET' DONE";
		return item;
	}
	return T();
}

// Only get if there is something to be got
template <typename T>
T ConcurrentQueue<T>::tryGet(bool &got)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		QMutexLocker ml(&mMutex);
		int count=mItems.count();
		if(count<=0) {
			got=false;
			return T();
		}
		//qDebug()<<"Got not-empty with count="<<count;
		const bool wasFull=(count >= mCapacity);
		T item=mItems.takeLast();
		got=true;
		count=mItems.count();
		const bool isFull=(count < mCapacity);
		//qDebug()<<"State mCapacity="<<mCapacity<<", wasFull="<<wasFull<<", isFull="<<isFull<<", count="<<count;
		if((mCapacity>0) && (wasFull) && (!isFull)) {
			//qDebug()<<"Signaling not-full";
			ml.unlock();
			mNotFull.wakeAll();
			ml.relock();
			emit mSignalEmitter.contentChanged();
		}
		//qDebug()<<"'GET' DONE";
		return item;
	}
	return T();
}


template <typename T>
int ConcurrentQueue<T>::count()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		QMutexLocker ml(&mMutex);
		const int count=mItems.count();
		return count;
	}
	return 0;
}


template <typename T>
bool ConcurrentQueue<T>::isEmpty()
{
	OC_METHODGATE();
	return count()<=0;
}


template <typename T>
bool ConcurrentQueue<T>::isDone()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mDone;
	}
	return true;
}

#endif
// CONCURRENTQUEUE_HPP
