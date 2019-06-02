#ifndef CONCURRENTQUEUE_HPP
#define CONCURRENTQUEUE_HPP


#include "utility/Standard.hpp"
#include "basic/AtomicBoolean.hpp"

#include <QMutex>
#include <QWaitCondition>
#include <QLinkedList>

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

public:
	explicit ConcurrentQueue(int capacity=0);
	virtual ~ConcurrentQueue();

public:
	void put(T item);
	T get();
	T tryGet(bool &got);
	int count();
	bool isEmpty();
};



////////////////////////////////////////////////////////////////////////////////


template <typename T>
ConcurrentQueue<T>::ConcurrentQueue(int capacity)
	: mCapacity(capacity)
	, mDone(false)
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
void ConcurrentQueue<T>::put(T item)
{
	OC_METHODGATE();
	{
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
		//qDebug()<<"'PUT' DONE";
	}
}

// Block until queue actually contains something before fetching the first item
template <typename T>
T ConcurrentQueue<T>::get()
{
	OC_METHODGATE();
	unsigned long rtto=1000;
	QMutexLocker ml(&mMutex);
	int count=mItems.count();
	//qDebug()<<"Waiting for not-empty with count="<<count;
	while((count<=0) && (!mDone)) {
		const bool wok=mNotEmpty.wait(&mMutex, rtto);
		if(!wok) {
			//qDebug()<<" ... done="<<mDone <<" count="<<count;
		}
		count=mItems.count();
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
	}
	//qDebug()<<"'GET' DONE";
	return item;
}



// Only get if there is something to be got
template <typename T>
T ConcurrentQueue<T>::tryGet(bool &got)
{
	OC_METHODGATE();
	QMutexLocker ml(&mMutex);
	int count=mItems.count();
	if(count<=0){
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
	}
	//qDebug()<<"'GET' DONE";
	return item;
}





template <typename T>
int ConcurrentQueue<T>::count()
{
	OC_METHODGATE();
	QMutexLocker ml(&mMutex);
	const int count=mItems.count();
	return count;
}


template <typename T>
bool ConcurrentQueue<T>::isEmpty()
{
	OC_METHODGATE();
	return count()<=0;
}


/*
		{
			QMutexLocker ml(&mTransactionLogMutex);
			const int count=mTransactionLog.count();
			if (count >0) {
				trans=mTransactionLog.takeLast();
				gotOne=true;
				//qDebug()<<" + DataStore::processTransactions() fetched transaction leaving behind "<<mTransactionLog.count();
				mTransactionLogNotFull.wakeAll();
			}
		}
		if(gotOne) {
			qDebug()<<" + DataStore::processTransactions() running transaction with type="<<trans.type()<<"	from thread "<<utility::currentThreadID();
			trans.run();
			qDebug()<<" + DataStore::processTransactions() done running transaction with type="<<trans.type()<<" from thread "<<utility::currentThreadID();
		} else {
			qDebug()<<" + DataStore::processTransactions() no transaction fetched, looping from thread "<<utility::currentThreadID();
		}
		return trans;

*/
#endif // CONCURRENTQUEUE_HPP
