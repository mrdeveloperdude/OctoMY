#include "HashstoreRecord.hpp"

#include "Hashstore.hpp"

#include <QDebug>

#include <QRegularExpression>

#include <QDataStream>


HashstoreRecord::HashstoreRecord(const HashstoreRecord &other)
	: d_ptr(new HashstoreRecordPrivate(other.d_func()->m_storage,other.d_func()->m_key))
{

}


HashstoreRecord::HashstoreRecord(Hashstore &storage, const QString key)
	: d_ptr(new HashstoreRecordPrivate(storage, key))
{
}

HashstoreRecord::HashstoreRecord(HashstoreRecordPrivate &dd)
	: d_ptr(&dd)
{
}


Hashstore &HashstoreRecord::storage()
{
	Q_D(HashstoreRecord);
	return d->m_storage;
}

const QString HashstoreRecord::key()
{
	Q_D(HashstoreRecord);
	return d->m_key;
}

HashstoreRecordState HashstoreRecord::state(){
	Q_D(HashstoreRecord);
	return d->state();
}



bool HashstoreRecord::put(const QByteArray &value){
	Q_D(HashstoreRecord);
	return d->put(value);
}


QByteArray HashstoreRecord::get(){
	Q_D(HashstoreRecord);
	return d->get();
}



HashstoreRecord::HashstoreRecord(HashstoreRecord && other) : HashstoreRecord() {
	swap(*this, other);
}

HashstoreRecord & HashstoreRecord::operator=(HashstoreRecord other) {
	swap(*this, other);
	return *this;
}

bool HashstoreRecord::operator==(HashstoreRecord &other){
	return ( (other.d_func() == this->d_func() ) || (other.key() == key() ) );
}


bool HashstoreRecord::operator!=(HashstoreRecord &other){
	return !operator==(other);
}

void swap(HashstoreRecord& first, HashstoreRecord& second) /* nothrow */ {
	using std::swap;
	swap(first.d_ptr, second.d_ptr);
}


//////////////////////////////////////////////////////////////////////////////


HashstoreRecordState HashstoreRecordPrivate::state(){
	return HashstoreRecordState(false,false,false,false);
}


static QString dirPath(QDir dir, QString key, quint8 levels){
	QString sep(QDir::separator());
	QString dirPath;
	quint8 level=0;
	for(;level<levels;++level){
		dirPath+=key[level]+sep;
	}
	QString ret=dir.absoluteFilePath(dirPath);
	//qDebug()<<"DIR: "<<dir.absolutePath()<<" + PATH: "<<dirPath<<" = ABS:"<<ret;
	return ret;
}


static const QRegularExpression reSanitizeKey("/[^0-9A-F]/"); // trimmed 40-digit upper-case hex string

//TODO: match and report instead of replace
static QString sanitizeKeyFilename(QString key) {
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


bool HashstoreRecordPrivate::put(const QByteArray &value){
	if(""==m_path){
		qWarning()<<"ERROR: path was invalid";
		return false;
	}
	if(!m_file.exists()){
		QDir dir(m_path);
		if(!dir.exists()){
			if(!dir.mkpath(m_path)){
				qWarning()<<"ERROR: could not create dir "<<dir;
				return false;
			}
			else{
				//qDebug()<<"MKDIR "<<m_path;
			}
		}
	}
	if(!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		qWarning()<<"ERROR: could not open file for writing: "<<m_path;
		return false;
	}
	m_file.write(value);
	m_file.close();
	return true;
}



QByteArray HashstoreRecordPrivate::get(){
	if(!m_file.exists()){
		return QByteArray();
	}
	if(!m_file.open(QIODevice::ReadOnly)){
		qWarning()<<"ERROR: could not open file for reading: "<<m_path<<". Reason:"<<m_file.errorString();
		return QByteArray();
	}
	QByteArray ret=m_file.readAll();
	//qDebug()<<"READ ALL: "<<ret<<m_file.size();
	m_file.close();
	return ret;
}



//////////////////////////////////////////////////////////////////////////////


void HashstoreRecord::allocateOnDisk(){
	QString const k=key();
	if(k.size()<Hashstore::DIR_LEVELS){
		qWarning()<<"ERROR: key was invalid";
	}
}


void HashstoreRecord::loadFromDisk(){
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()){
			allocateOnDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()){
			qWarning()<<"ERROR: allocation of key "<<key()<<" failed";
			return;
		}
		if(!s.onDisk()){
			loadFromDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.onDisk()){
			qWarning()<<"ERROR: loading of data for key "<<key()<<" into memory failed";
			return;
		}
	}
}

void HashstoreRecord::saveToDisk(){
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()){
			allocateOnDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.keyExists()){
			qWarning()<<"ERROR: allocation of key "<<key()<<" failed";
			return;
		}
		if(!s.onDisk()){
			loadFromDisk();
		}
	}
	{
		HashstoreRecordState s=state();
		if(!s.onDisk()){
			qWarning()<<"ERROR: loading of data for key "<<key()<<" into memory failed";
			return;
		}
	}
}



