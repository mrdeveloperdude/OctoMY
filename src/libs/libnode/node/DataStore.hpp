#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "basic/AtomicBoolean.hpp"
#include "utility/Standard.hpp"

#include <QObject>
#include <QSharedPointer>

#include <QRunnable>
#include <QThread>

#include <QDebug>



#include <QDebug>
#include <QThread>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <QWaitCondition>


/*

  DataStore is used to persist data in form of variant maps.

  The long term goal of the API is to ensure compliance with ACID requirements as far as
  possible while also being asynchronous.

  The API should also be as transparent as possible, meaning that using it should
  require the least possible amount of boilerplate.

  ACID is an acronym for the following 4 requirements:

  + Atomicity    - The operation is indivisible, all-or-nothing. A transaction can be "committed" or "rolled back"; if it is committed all of its effects must complete fully, otherwise if it is rolled back it must have no effect.
  + Consistency  - The transaction transforms the database from one consistent state to another consistent state. Here "consistent" implies obeying not just relational integrity constraints but also business rules and semantics, such as that a given Customer record's Address and ZIP fields must not specify inconsistent information (e.g., an address in New York and a ZIP code in Alabama).
  + Isolation    - Two transactions are fully isolated if they can be started simultaneously and the result is guaranteed to be the same as though one was run entirely before the other ("Serializable"). In practice, databases support various levels of isolation, each of which trades off better isolation at the expense of concurrency or vice versa. Some typical isolation levels, in order from best to worst concurrency, are: Read Uncommitted (a.k.a. Dirty Read), Read Committed, Repeatable Read, and Serializable (highest isolation), and describe what kinds of intermediate states of other transactions might be visible.
  + Durability   - If a transaction is committed, its effects are not lost even if there is a subsequent system crash. This requirement is usually implemented using a transaction log which is used to "roll forward" the state of the database on system restart.

*/


enum TransactionType {
	TRANSACTION_CLEAR, TRANSACTION_GET, TRANSACTION_SET, TRANSACTION_LOAD, TRANSACTION_SAVE, TRANSACTION_SYNCHRONIZE
};


class Transaction;
class DataStore;


/**
  Qt Style private class for Transaction
  ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
*/



class TransactionPrivate
{
private:
	DataStore & mStore;
	const TransactionType mType;
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
	explicit TransactionPrivate(DataStore & store, const TransactionType type, QVariantMap data=QVariantMap());
	virtual ~TransactionPrivate() {}

public:

	friend class Transaction;
};


////////////////////////////////////////////////////////////////////////////////


class Transaction
{
private:
	QScopedPointer<TransactionPrivate>  d_ptr;
	Q_DECLARE_PRIVATE(Transaction)

public:
	inline Transaction() Q_DECL_NOTHROW{}
	virtual ~Transaction() {}
public:
	explicit Transaction(DataStore & store, const TransactionType type, QVariantMap data=QVariantMap());
	explicit Transaction(TransactionPrivate &dd);

	Transaction(const Transaction & other);
	Transaction(Transaction && other);

	Transaction & operator=(Transaction other);
	bool operator==(Transaction &other);
	bool operator!=(Transaction &other);

	friend void swap(Transaction& first, Transaction& second) /* nothrow */;


public:
	DataStore & store() const ;
	TransactionType type() const ;
	QVariantMap data() const ;
	bool isStarted();

	bool isFinished();
	void waitForFinished();
	template <typename F>
	void onFinished(F callBack);

	bool isSuccessfull();
	QString message();
	bool run();


};



template <typename F>
void Transaction::onFinished(F callBack)
{
	OC_METHODGATE()
	waitForFinished();
	callBack(*this);
}


////////////////////////////////////////////////////////////////////////////////




class DataStore: public QObject
{
	Q_OBJECT
protected:
	QVariantMap mData;
	QString mFilename;

	AtomicBoolean mThreadStarted;
	QMutex mThreadStartedMutex;
	QWaitCondition mThreadStartedWait;

	QMutex mTransactionLogMutex;
	QWaitCondition mTransactionLogNotEmpty;
	QWaitCondition mTransactionLogNotFull;

	QList<Transaction> mTransactionLog;

	QAtomicInteger<quint64> mAutoIncrement;
	QAtomicInteger<quint64> mDiskCounter;
	QAtomicInteger<quint64> mMemoryCounter;
	AtomicBoolean mDone;


public:
	explicit DataStore(QString filename="", QObject *parent = nullptr);
	virtual ~DataStore();


public:

	QString filename() const;
	bool fileExists() const;
	bool ready();


private:

	Transaction enqueueTransaction(Transaction trans);
	void processTransactions();

	quint64 autoIncrement();

public:
	Transaction clear();
	Transaction get();
	Transaction set(QVariantMap data);
	Transaction load();
	Transaction save();
	Transaction synchronize();


private:

	// Delete data on disk and in memory
	bool clearSync();
	// Get data from memory
	QVariantMap getSync();
	// Set data in memory
	bool setSync(QVariantMap data);
	// Load data from file on disk into memory
	bool loadSync();
	// Save data from memory into file on disk
	bool saveSync();

	// Save or load data depending on wether memory or disk is newest
	bool synchronizeSync();


public:

	friend class Transaction;
	friend class TransactionPrivate;
	friend class SimpleDataStore;
	friend const QDebug &operator<<(QDebug &d, DataStore &ks);
};


const QDebug &operator<<(QDebug &d, DataStore &ks);


////////////////////////////////////////////////////////////////////////////////




// Convenience wrapper for DataStore
class SimpleDataStore
{
private:
	DataStore mDataStore;

public:

	explicit SimpleDataStore(QString filename);
	virtual ~SimpleDataStore();

	// SimpleDataStore interface
public:
	// NOTE: Must accept empty map, beacuse calling fromMap with empty map is the semantic for clearing
	virtual bool fromMap(QVariantMap data)=0;
	virtual QVariantMap toMap() =0;

public:

	QString filename() const;
	bool fileExists() const;
	bool ready();

	template <typename F>
	void clear(F callBack);
	template <typename F>
	void save(F callBack);
	template <typename F>
	void load(F callBack);
	template <typename F>
	void synchronize(F callBack);

	void clear();
	void save();
	void load();
	void synchronize();
};

template <typename F>
void SimpleDataStore::clear(F callBack)
{
	OC_METHODGATE();
	mDataStore.clear().onFinished([=](Transaction t) {
		fromMap(QVariantMap());
		callBack(*this, t.isSuccessfull());
	});
}

template <typename F>
void SimpleDataStore::save(F callBack)
{
	OC_METHODGATE();
	mDataStore.set(toMap());
	mDataStore.save().onFinished([=](Transaction t) {
		callBack(*this, t.isSuccessfull());
	});
}



template <typename F>
void SimpleDataStore::load(F callBack)
{
	OC_METHODGATE();
	mDataStore.load().onFinished([=](Transaction t) {
		if(t.isSuccessfull()) {
			mDataStore.get().onFinished([=](Transaction t) {
				fromMap(t.data());
				callBack(*this, t.isSuccessfull());
			});
		} else {
			callBack(*this, false);
		}
	});

}



template <typename F>
void SimpleDataStore::synchronize(F callBack)
{
	OC_METHODGATE();
	mDataStore.synchronize().onFinished([=](Transaction t) {
		if(t.isSuccessfull()) {
			mDataStore.get().onFinished([=](Transaction t) {
				fromMap(t.data());
				callBack(*this, t.isSuccessfull());
			});
		} else {
			callBack(*this, false);
		}
	});
}




////////////////////////////////////////////////////////////////////////////////


class SimpleMapStore: public SimpleDataStore
{

private:
	QVariantMap mMap;
public:
	explicit SimpleMapStore(QString filename);
	virtual ~SimpleMapStore();

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data)  Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;
};


#endif // DATASTORE_HPP
