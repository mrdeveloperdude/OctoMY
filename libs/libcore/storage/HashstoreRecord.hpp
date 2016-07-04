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



/**
  Qt Style private class for HashstoreRecord
  ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
*/
class HashstoreRecordPrivate{

	private:

		Hashstore &m_storage;
		const QString m_key;
		const QString m_path;
		QFile m_file;
		const quint16 m_flags;

	public:

		explicit HashstoreRecordPrivate(Hashstore &storage, const QString key);
		virtual ~HashstoreRecordPrivate(){}
	public:


		HashstoreRecordState state();

		bool put(const QByteArray &value);
		QByteArray get();

		friend class HashstoreRecord;
};

/**
  HashstoreRecord is a "reusable future" that wraps each record in hasstore
  It has a QtStyle private class so it can easily be copied by value
*/

class HashstoreRecord{

	private:
		QScopedPointer<HashstoreRecordPrivate>  d_ptr;
		Q_DECLARE_PRIVATE(HashstoreRecord)



	public:
		inline HashstoreRecord() Q_DECL_NOTHROW{}
		virtual ~HashstoreRecord(){}
	public:
		explicit HashstoreRecord(Hashstore &storage, const QString key);
		explicit HashstoreRecord(HashstoreRecordPrivate &dd);

		HashstoreRecord(const HashstoreRecord & other);
		HashstoreRecord(HashstoreRecord && other);

		HashstoreRecord & operator=(HashstoreRecord other);
		bool operator==(HashstoreRecord &other);
		bool operator!=(HashstoreRecord &other);

		friend void swap(HashstoreRecord& first, HashstoreRecord& second) /* nothrow */;

		inline bool keyExists()	{Q_D(HashstoreRecord); return (d->m_flags&1)>0; } // Is this record pointing to a valid piece of data?
		inline bool inMemory()	{Q_D(HashstoreRecord); return (d->m_flags&2)>0;} // Is the latest incarnation of data for this record in memory?
		inline bool onDisk()	{Q_D(HashstoreRecord); return (d->m_flags&4)>0;} // Is the latest incarnation of data for this record stored safely on disk?
		inline bool diskSynced(){Q_D(HashstoreRecord); return (d->m_flags&8)>0;} // Has disk been synchronized since latest incarnation of data for this record was stored safely on disk?

	private:
		void allocateOnDisk();
		void loadFromDisk();
		void saveToDisk();
	public:
		Hashstore &storage();
		const QString key();
		HashstoreRecordState state();

		bool put(const QByteArray &value);
		QByteArray get();

	public:
		friend class Hashstore;


};




#endif // HASHSTORERECORD_HPP
