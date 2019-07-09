#ifndef HASHSTORERECORD_HPP
#define HASHSTORERECORD_HPP


#include "HashstoreRecordState.hpp"


#include <QLockFile>
#include <QScopedPointer>
#include <QHash>
#include <QFile>

class QDir;
class QByteArray;
class QString;

class Hashstore;

class HashstoreRecordPrivate;


/**
  HashstoreRecord is a "reusable future" that wraps each record in hasstore
  It has a Qt style private class so it can easily be copied by value
*/

class HashstoreRecord
{

private:
	//QScopedPointer<HashstoreRecordPrivate>  d_ptr;
	HashstoreRecordPrivate  *d_ptr;
	Q_DECLARE_PRIVATE(HashstoreRecord)

public:
	inline HashstoreRecord() Q_DECL_NOTHROW{}
	virtual ~HashstoreRecord() {}

public:
	explicit HashstoreRecord(Hashstore &storage, const QString key);
	explicit HashstoreRecord(HashstoreRecordPrivate &dd);

	HashstoreRecord(const HashstoreRecord & other);
	HashstoreRecord(HashstoreRecord && other);

	HashstoreRecord & operator=(HashstoreRecord other);
	bool operator==(HashstoreRecord &other);
	bool operator!=(HashstoreRecord &other);

	friend void swap(HashstoreRecord& first, HashstoreRecord& second) /* nothrow */;

public:
	inline bool keyExists();
	inline bool inMemory();
	inline bool onDisk();
	inline bool diskSynced();

private:
	void allocateOnDisk();
	void loadFromDisk();
	void saveToDisk();

public:
	Hashstore &storage();
	const QString key();
	HashstoreRecordState state();

public:
	bool put(const QByteArray &value);
	QByteArray get();

public:
	friend class Hashstore;


};




#endif
// HASHSTORERECORD_HPP
