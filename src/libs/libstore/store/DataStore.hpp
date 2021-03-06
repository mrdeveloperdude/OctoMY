#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#ifdef USE_OLD_STORE

#include "utility/Standard.hpp"
#include "utility/ConcurrentQueue.hpp"

#include "node/StorageEvent.hpp"

//#include "node/DataStoreInterface.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QObject>

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


////////////////////////////////////////////////////////////////////////////////


class DataStore
{
protected:

	QVariantMap mData;
	QString mFilename;


	ConcurrentQueue<StorageEvent> mTransactions;

	QAtomicInteger<quint64> mAutoIncrement;
	QAtomicInteger<quint64> mDiskCounter;
	QAtomicInteger<quint64> mMemoryCounter;
	AtomicBoolean mDone;

	QFuture<void> mCompleteFuture;


public:
	explicit DataStore(QString filename="", QObject *parent = nullptr);
	virtual ~DataStore();


public:

	QString filename() const;
	bool fileExists() const;
	bool ready();


private:

	StorageEvent enqueueTransaction(StorageEvent trans);
	void processTransactions();

	quint64 autoIncrement();

public:
	StorageEvent clear();
	StorageEvent get();
	StorageEvent set(QVariantMap data);
	StorageEvent load();
	StorageEvent save();
	StorageEvent bootstrap();
	StorageEvent synchronize();

private:

	StorageEvent complete();


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
	// Bootstrap data into memory
	bool bootstrapSync();

	// Save, load  or bootstrap data depending on wether memory or disk is newest
	bool synchronizeSync();

	// Internal transaction to stop transaction processing thread
	bool completeSync();


public:

	friend class StorageEvent;
	friend class StorageEventPrivate;
	friend class SimpleDataStore;
	friend const QDebug &operator<<(QDebug &d, DataStore &ks);
};



#endif

#endif // DATASTORE_HPP
