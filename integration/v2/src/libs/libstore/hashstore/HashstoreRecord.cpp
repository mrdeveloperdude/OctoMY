#include "HashstoreRecord.hpp"
#include "HashstoreRecordPrivate.hpp"

#include "Hashstore.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <QDataStream>


HashstoreRecord::HashstoreRecord(const HashstoreRecord &other)
	: d_ptr(OC_NEW HashstoreRecordPrivate(other.d_func()->m_storage,other.d_func()->m_key))
{
	OC_METHODGATE();

}


HashstoreRecord::HashstoreRecord(Hashstore &storage, const QString key)
	: d_ptr(OC_NEW HashstoreRecordPrivate(storage, key))
{
	OC_METHODGATE();
}

HashstoreRecord::HashstoreRecord(HashstoreRecordPrivate &dd)
	: d_ptr(&dd)
{
	OC_METHODGATE();
}


Hashstore &HashstoreRecord::storage()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);
	return d->m_storage;
}

const QString HashstoreRecord::key()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);
	return d->m_key;
}

HashstoreRecordState HashstoreRecord::state()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);
	return d->state();
}



bool HashstoreRecord::put(const QByteArray &value)
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);
	return d->put(value);
}


QByteArray HashstoreRecord::get()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);
	return d->get();
}



HashstoreRecord::HashstoreRecord(HashstoreRecord && other)
	: HashstoreRecord()
{
	OC_METHODGATE();
	swap(*this, other);
}

HashstoreRecord & HashstoreRecord::operator=(HashstoreRecord other)
{
	OC_METHODGATE();
	swap(*this, other);
	return *this;
}

bool HashstoreRecord::operator==(HashstoreRecord &other)
{
	OC_METHODGATE();
	return ( (other.d_func() == this->d_func() ) || (other.key() == key() ) );
}


bool HashstoreRecord::operator!=(HashstoreRecord &other)
{
	OC_METHODGATE();
	return !operator==(other);
}

void swap(HashstoreRecord& first, HashstoreRecord& second) /* nothrow */
{
	OC_FUNCTIONGATE();
	using std::swap;
	swap(first.d_ptr, second.d_ptr);
}



inline bool HashstoreRecord::keyExists()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);    // Is this record pointing to a valid piece of data?
	return (d->m_flags&1)>0;
}
inline bool HashstoreRecord::inMemory()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);    // Is the latest incarnation of data for this record in memory?
	return (d->m_flags&2)>0;
}
inline bool HashstoreRecord::onDisk()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);    // Is the latest incarnation of data for this record stored safely on disk?
	return (d->m_flags&4)>0;
}
inline bool HashstoreRecord::diskSynced()
{
	OC_METHODGATE();
	Q_D(HashstoreRecord);    // Has disk been synchronized since latest incarnation of data for this record was stored safely on disk?
	return (d->m_flags&8)>0;
}


//////////////////////////////////////////////////////////////////////////////


HashstoreRecordState HashstoreRecordPrivate::state()
{
	OC_METHODGATE();
	return HashstoreRecordState(false,false,false,false);
}


static QString dirPath(QDir dir, QString key, quint8 levels)
{
	OC_FUNCTIONGATE();
	QString sep(QDir::separator());
	QString dirPath;
	quint8 level=0;
	for(; level<levels; ++level) {
		dirPath+=key[level]+sep;
	}
	QString ret=dir.absoluteFilePath(dirPath);
	//qDebug()<<"DIR: "<<dir.absolutePath()<<" + PATH: "<<dirPath<<" = ABS:"<<ret;
	return ret;
}


static const QRegularExpression reSanitizeKey("/[^0-9A-F]/"); // trimmed 40-digit upper-case hex string

//TODO: match and report instead of replace
static QString sanitizeKeyFilename(QString key)
{
	OC_FUNCTIONGATE();
	return key.replace(reSanitizeKey,"_");
}

HashstoreRecordPrivate::HashstoreRecordPrivate(Hashstore &storage, const QString key)
	: m_storage(storage)
	, m_key(sanitizeKeyFilename(key))
	, m_path(dirPath(storage.dir(), m_key, Hashstore::DIR_LEVELS))
	, m_file(m_path + m_key)
	, m_flags(0)
{

}


bool HashstoreRecordPrivate::put(const QByteArray &value)
{
	OC_METHODGATE();
	if(""==m_path) {
		qWarning()<<"ERROR: path was invalid";
		return false;
	}
	if(!m_file.exists()) {
		QDir dir(m_path);
		if(!dir.exists()) {
			if(!dir.mkpath(m_path)) {
				qWarning()<<"ERROR: could not create dir "<<dir;
				return false;
			} else {
				//qDebug()<<"MKDIR "<<m_path;
			}
		}
	}
	if(!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning()<<"ERROR: could not open file for writing: "<<m_path;
		return false;
	}
	m_file.write(value);
	m_file.close();
	return true;
}



QByteArray HashstoreRecordPrivate::get()
{
	OC_METHODGATE();
	if(!m_file.exists()) {
		return QByteArray();
	}
	if(!m_file.open(QIODevice::ReadOnly)) {
		qWarning()<<"ERROR: could not open file for reading: "<<m_path<<". Reason:"<<m_file.errorString();
		return QByteArray();
	}
	QByteArray ret=m_file.readAll();
	//qDebug()<<"READ ALL: "<<ret<<m_file.size();
	m_file.close();
	return ret;
}



//////////////////////////////////////////////////////////////////////////////


void HashstoreRecord::allocateOnDisk()
{
	OC_METHODGATE();
	QString const k=key();
	if(k.size()<Hashstore::DIR_LEVELS) {
		qWarning()<<"ERROR: key was invalid";
	}
}


void HashstoreRecord::loadFromDisk()
{
	OC_METHODGATE();
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()) {
			allocateOnDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()) {
			qWarning()<<"ERROR: allocation of key "<<key()<<" failed";
			return;
		}
		if(!s.onDisk()) {
			loadFromDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.onDisk()) {
			qWarning()<<"ERROR: loading of data for key "<<key()<<" into memory failed";
			return;
		}
	}
}

void HashstoreRecord::saveToDisk()
{
	OC_METHODGATE();
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()) {
			allocateOnDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()) {
			qWarning()<<"ERROR: allocation of key "<<key()<<" failed";
			return;
		}
		if(!s.onDisk()) {
			loadFromDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.onDisk()) {
			qWarning()<<"ERROR: loading of data for key "<<key()<<" into memory failed";
			return;
		}
	}
}



