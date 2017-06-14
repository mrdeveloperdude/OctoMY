#ifndef HASHSTORE_HPP
#define HASHSTORE_HPP

#include "HashstoreRecord.hpp"

#include <QString>
#include <QLockFile>
#include <QHash>
#include <QDir>

/**
  ZooStorage is a storage manager class that is the starting point for the
  persistent storage API for Zoo.
*/
class Hashstore{
	private:
		// Use lockfile to prevent several instances using same storage by mistake
		QDir m_dir;
		const QString lockFilename;
		QLockFile lock;
		// Keep "index" in memory
		QHash<QString, HashstoreRecord> index;

	public:

		static const quint8 DIR_LEVELS;

	public:
		explicit Hashstore(QDir dir);
		virtual ~Hashstore();
	public:

		const  QDir dir() const ;
		HashstoreRecord resolve(const QString &key);
		QString generatePathFromKey(const QString &key) const;

};



#endif // HASHSTORE_HPP
