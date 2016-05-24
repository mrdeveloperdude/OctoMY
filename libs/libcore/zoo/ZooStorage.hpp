#ifndef ZOOSTORAGE_HPP
#define ZOOSTORAGE_HPP

#include "ZooRecord.hpp"

#include <QString>
#include <QLockFile>
#include <QHash>
#include <QDir>

/**
  ZooStorage is a storage manager class that is the starting point for the
  persistent storage API for Zoo.
*/
class ZooStorage{
	private:
		// Use lockfile to prevent several instances using same storage by mistake
		QDir m_dir;
		const QString lockFilename;
		QLockFile lock;
		// Keep "index" in memory
		QHash<QString, ZooRecord> index;

	public:

		static const quint8 DIR_LEVELS;

	public:
		explicit ZooStorage(QDir dir);
		virtual ~ZooStorage();
	public:

		const  QDir dir() const ;
		ZooRecord resolve(const QString &key);
		QString generatePathFromKey(const QString &key) const;

};



#endif // ZOOSTORAGE_HPP
