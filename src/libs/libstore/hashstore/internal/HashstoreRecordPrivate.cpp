#include "HashstoreRecordPrivate.hpp"
#include "Hashstore.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QDir>
#include <QRegularExpression>


HashstoreRecordPrivate::HashstoreRecordPrivate(const HashstoreRecordPrivate &other)
{
	OC_METHODGATE();
	Q_UNUSED(other);
}


HashstoreRecordPrivate::HashstoreRecordPrivate(Hashstore &storage, const QString key)
{
	OC_METHODGATE();
	Q_UNUSED(storage);
	Q_UNUSED(key);
}

HashstoreRecordPrivate::HashstoreRecordPrivate(HashstoreRecordPrivatePrivate &dd)
{
	OC_METHODGATE();
	Q_UNUSED(dd);
}


Hashstore &HashstoreRecordPrivate::storage()
{
	OC_METHODGATE();
	Q_D(HashstoreRecordPrivate);
	return d->m_storage;
}

const QString HashstoreRecordPrivate::key()
{
	OC_METHODGATE();
	Q_D(HashstoreRecordPrivate);
	return d->m_key;
}

HashstoreRecordPrivateState HashstoreRecordPrivate::state()
{
	OC_METHODGATE();
	Q_D(HashstoreRecordPrivate);
	return d->state();
}



bool HashstoreRecordPrivate::put(const QByteArray &value)
{
	OC_METHODGATE();
	Q_D(HashstoreRecordPrivate);
	return d->put(value);
}


QByteArray HashstoreRecordPrivate::get()
{
	OC_METHODGATE();
	Q_D(HashstoreRecordPrivate);
	return d->get();
}



HashstoreRecordPrivate::HashstoreRecordPrivate(HashstoreRecordPrivate && other)
	: HashstoreRecordPrivate()
{
	OC_METHODGATE();
	swap(*this, other);
}

HashstoreRecordPrivate & HashstoreRecordPrivate::operator=(HashstoreRecordPrivate other)
{
	OC_METHODGATE();
	swap(*this, other);
	return *this;
}

bool HashstoreRecordPrivate::operator==(HashstoreRecordPrivate &other)
{
	OC_METHODGATE();
	return ( (other.d_func() == this->d_func() ) || (other.key() == key() ) );
}


bool HashstoreRecordPrivate::operator!=(HashstoreRecordPrivate &other)
{
	OC_METHODGATE();
	return !operator==(other);
}

void swap(HashstoreRecordPrivate& first, HashstoreRecordPrivate& second) /* nothrow */
{
	OC_FUNCTIONGATE();
	using std::swap;
	swap(first.d_ptr, second.d_ptr);
}


//////////////////////////////////////////////////////////////////////////////


HashstoreRecordPrivateState HashstoreRecordPrivatePrivate::state()
{
	OC_METHODGATE();
	return HashstoreRecordPrivateState(false,false,false,false);
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

HashstoreRecordPrivatePrivate::HashstoreRecordPrivatePrivate(Hashstore &storage, const QString key)
	: m_storage(storage)
	, m_key(sanitizeKeyFilename(key))
	, m_path(dirPath(storage.dir(), m_key, Hashstore::DIR_LEVELS))
	, m_file(m_path + m_key)
	, m_flags(0)
{

}


bool HashstoreRecordPrivatePrivate::put(const QByteArray &value)
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



QByteArray HashstoreRecordPrivatePrivate::get()
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


