#include "ZooRecord.hpp"

#include "ZooStorage.hpp"

#include <QDebug>

#include <QRegularExpression>

#include <QDataStream>


ZooRecord::ZooRecord(const ZooRecord &other)
	: d_ptr(new ZooRecordPrivate(other.d_func()->m_storage,other.d_func()->m_key))
{

}


ZooRecord::ZooRecord(ZooStorage &storage, const QString key)
	: d_ptr(new ZooRecordPrivate(storage, key))
{
}

ZooRecord::ZooRecord(ZooRecordPrivate &dd)
	: d_ptr(&dd)
{
}


ZooStorage &ZooRecord::storage()
{
	Q_D(ZooRecord);
	return d->m_storage;

}

QString ZooRecord::key()
{
	Q_D(ZooRecord);
	return d->m_key;
}

ZooRecordState ZooRecord::state(){
	Q_D(ZooRecord);
	return d->state();
}



bool ZooRecord::put(const QByteArray &value){
	Q_D(ZooRecord);
	return d->put(value);
}


QByteArray ZooRecord::get(){
	Q_D(ZooRecord);
	return d->get();
}



ZooRecord::ZooRecord(ZooRecord && other) : ZooRecord() {
	swap(*this, other);
}

ZooRecord & ZooRecord::operator=(ZooRecord other) {
	swap(*this, other);
	return *this;
}


void swap(ZooRecord& first, ZooRecord& second) /* nothrow */ {
	using std::swap;
	swap(first.d_ptr, second.d_ptr);
}


//////////////////////////////////////////////////////////////////////////////


ZooRecordState ZooRecordPrivate::state(){
	return ZooRecordState(false,false,false,false);
}


static QString dirPath(QDir dir, QString key, quint8 levels){
	QString sep(QDir::separator());
	QString dirPath;
	quint8 level=0;
	for(;level<levels;++level){
		dirPath+=key[level]+sep;
	}
	QString ret=dir.absoluteFilePath(dirPath);
	qDebug()<<"DIR: "<<dir.absolutePath()<<" + PATH: "<<dirPath<<" = ABS:"<<ret;
	return ret;
}


static const QRegularExpression reSanitizeKey("/[^0-9A-F]/"); // trimmed 40-digit upper-case hex string

static QString sanitizeKeyFilename(QString key) {
	return key.replace(reSanitizeKey,"_");
}

ZooRecordPrivate::ZooRecordPrivate(ZooStorage &storage, const QString key)
	: m_storage(storage)
	, m_key(sanitizeKeyFilename(key))
	, m_path(dirPath(storage.dir(), m_key, ZooStorage::DIR_LEVELS))
	, m_file(m_path + m_key)
{

}


bool ZooRecordPrivate::put(const QByteArray &value){
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
				qDebug()<<"MKDIR "<<m_path;
			}
		}
	}
	if(!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		qWarning()<<"ERROR: could not open file for writing: "<<m_path;
		return false;
	}
	QDataStream out(&m_file);
	out << value;
	return true;
}



QByteArray ZooRecordPrivate::get(){
	if(!m_file.exists()){
		return QByteArray();
	}
	if(!m_file.open(QIODevice::ReadOnly)){
		qWarning()<<"ERROR: could not open file for reading: "<<m_path;
		return QByteArray();
	}
	QByteArray ret=m_file.readAll();
	m_file.close();
	return ret;
}



//////////////////////////////////////////////////////////////////////////////


void ZooRecord::allocateOnDisk(){
	QString const k=key();
	if(k.size()<ZooStorage::DIR_LEVELS){
		qWarning()<<"ERROR: key was invalid";
	}
}


void ZooRecord::loadFromDisk(){
	{
		ZooRecordState s=state();
		if(!s.keyExists()){
			allocateOnDisk();
		}
	}
	{
		ZooRecordState s=state();
		if(!s.keyExists()){
			qWarning()<<"ERROR: allocation of key "<<key()<<" failed";
			return;
		}
		if(!s.onDisk()){
			loadFromDisk();
		}
	}
	{
		ZooRecordState s=state();
		if(!s.onDisk()){
			qWarning()<<"ERROR: loading of data for key "<<key()<<" into memory failed";
			return;
		}
	}
}

void ZooRecord::saveToDisk(){
	{
		ZooRecordState s=state();
		if(!s.keyExists()){
			allocateOnDisk();
		}
	}
	{
		ZooRecordState s=state();
		if(!s.keyExists()){
			qWarning()<<"ERROR: allocation of key "<<key()<<" failed";
			return;
		}
		if(!s.onDisk()){
			loadFromDisk();
		}
	}
	{
		ZooRecordState s=state();
		if(!s.onDisk()){
			qWarning()<<"ERROR: loading of data for key "<<key()<<" into memory failed";
			return;
		}
	}
}



