#ifndef HASHSTORERECORDPRIVATE_HPP
#define HASHSTORERECORDPRIVATE_HPP



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
class HashstoreRecordPrivate
{

private:

	Hashstore &m_storage;
	const QString m_key;
	const QString m_path;
	QFile m_file;
	const quint16 m_flags;

public:

	explicit HashstoreRecordPrivate(Hashstore &storage, const QString key);
	virtual ~HashstoreRecordPrivate() {}
public:


	HashstoreRecordState state();

	bool put(const QByteArray &value);
	QByteArray get();

	friend class HashstoreRecord;
};

#endif
// HASHSTORERECORDPRIVATE_HPP
