#ifndef STORAGEEVENT_HPP
#define STORAGEEVENT_HPP

#include "utility/Standard.hpp"

#include <QSharedDataPointer>

#include <QWaitCondition>
#include <QVariantMap>
#include <QMutex>


enum StorageEventType: quint8 {
	STORAGE_EVENT_CLEAR, STORAGE_EVENT_GET, STORAGE_EVENT_SET, STORAGE_EVENT_LOAD, STORAGE_EVENT_SAVE, STORAGE_EVENT_SYNCHRONIZE, STORAGE_EVENT_DONE
};


class StorageEvent;
class DataStore;



QDebug operator<< (QDebug d, StorageEventType tt);


/**
  Private class for Transaction handle
  ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
*/



class StorageEventPrivate
{
private:
	DataStore & mStore;
	const StorageEventType mType;
	QVariantMap mData;
	QMutex mStartedMutex;
	QMutex mFinishedMutex;
	QWaitCondition mFinishedCond;
	QMutex mSuccessfullMutex;
	QMutex mMessageMutex;
	bool mStarted;
	bool mFinished;
	bool mSuccessfull;
	QString mMessage;


public:
	explicit StorageEventPrivate(DataStore & store, const StorageEventType type, QVariantMap data=QVariantMap());
	virtual ~StorageEventPrivate() {}

public:

	friend class StorageEvent;
};



////////////////////////////////////////////////////////////////////////////////




class StorageEvent
{
private:
	QSharedPointer<StorageEventPrivate>  p_ptr;

public:
	inline StorageEvent() Q_DECL_NOTHROW{}
	virtual ~StorageEvent() {}
public:
	explicit StorageEvent(DataStore & store, const StorageEventType type, QVariantMap data=QVariantMap());
	explicit StorageEvent(StorageEventPrivate &pp);

	StorageEvent(const StorageEvent & other);
	StorageEvent(StorageEvent && other);

	StorageEvent & operator=(StorageEvent other);
	bool operator==(StorageEvent &other);
	bool operator!=(StorageEvent &other);

	//friend void swap(Transaction& first, Transaction& second) Q_DECL_NOTHROW;


public:
	DataStore & store() const ;
	StorageEventType type() const ;
	QVariantMap data() const ;
	bool isStarted();

	bool isFinished();
	void notifyFinished();
	void waitForFinished();
	template <typename F>
	void onFinished(F callBack);

	bool isSuccessfull();
	QString message();
	bool run();


};


// NOTE: Will block, which may not be what you expect or want
template <typename F>
void StorageEvent::onFinished(F callBack)
{
	OC_METHODGATE();
	waitForFinished();
	callBack(*this);
}



const QDebug &operator<<(QDebug &d, DataStore &ks);

#endif // STORAGEEVENT_HPP
