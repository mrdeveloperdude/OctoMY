#ifndef ZOORECORD_HPP
#define ZOORECORD_HPP


#include "ZooRecordState.hpp"


#include <QLockFile>
#include <QScopedPointer>
#include <QHash>
#include <QFile>

class QDir;
class QByteArray;
class QString;

class ZooStorage;



/**
  Qt Style private class for ZooRecord
*/
class ZooRecordPrivate{

	private:

		ZooStorage &m_storage;
		const QString m_key;
		const QString m_path;
		QFile m_file;

	public:

		explicit ZooRecordPrivate(ZooStorage &storage, const QString key);
		virtual ~ZooRecordPrivate(){}
	public:


		ZooRecordState state();

		bool put(const QByteArray &value);
		QByteArray get();

		friend class ZooRecord;
};

/**
  ZooRecord is a "reusable future" that wraps each record in zoo storage
  It has a QtStyle private class so it can easily be copied by value
*/

class ZooRecord{

	private:
		QScopedPointer<ZooRecordPrivate>  d_ptr;
		Q_DECLARE_PRIVATE(ZooRecord)


	public:
		inline ZooRecord() Q_DECL_NOTHROW{}
		virtual ~ZooRecord(){}
	public:
		explicit ZooRecord(ZooStorage &storage, const QString key);
		explicit ZooRecord(ZooRecordPrivate &dd);

		ZooRecord(const ZooRecord & other);
		ZooRecord(ZooRecord && other);

		ZooRecord & operator=(ZooRecord other);

		friend void swap(ZooRecord& first, ZooRecord& second) /* nothrow */;


	private:
		void allocateOnDisk();
		void loadFromDisk();
		void saveToDisk();
	public:
		ZooStorage &storage();
		QString key();
		ZooRecordState state();

		bool put(const QByteArray &value);
		QByteArray get();

	public:
		friend class ZooStorage;


};




#endif // ZOORECORD_HPP
