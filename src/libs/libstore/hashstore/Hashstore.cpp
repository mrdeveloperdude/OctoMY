#include "Hashstore.hpp"

#include "uptime/MethodGate.hpp"

#include <QLockFile>

#include <QString>
#include <QByteArray>
#include <QDebug>


HashstoreRecord::HashstoreRecord(const Hashstore &hs, const QString key){
	OC_METHODGATE();
	Q_UNUSED(hs);
	Q_UNUSED(key);
}

const quint8 Hashstore::DIR_LEVELS=4;

Hashstore::Hashstore(QDir dir)
	: m_dir(dir)
	, lockFilename(dir.absoluteFilePath(QStringLiteral("zoo_storage.lock")))
	, lock(lockFilename)
{
	OC_METHODGATE();
	if(!lock.tryLock(1000)){
		qint64 pid=0;
		QString hostname;
		QString appname;
		lock.getLockInfo(&pid, &hostname, &appname);
		qWarning()<<"ERROR: Could not lock on "<<lockFilename<<". Reason: "<<lock.error()<<", pid="<<pid<<", hostname="<<hostname<<", appname="<<appname;
	}
}


Hashstore::~Hashstore(){
	OC_METHODGATE();
	lock.unlock();
}


const QDir Hashstore::dir() const {
	OC_METHODGATE();
	return m_dir;
}


HashstoreRecord Hashstore::resolve(const QString &key){
	OC_METHODGATE();
	if(index.contains(key)){
		return index[key];
	}
	else{
		HashstoreRecord rec(*this, key);
		index.insert(key,rec);
		return rec;
	}

}


QString Hashstore::generatePathFromKey(const QString &key) const{
	OC_METHODGATE();
	if(key.size()<DIR_LEVELS){
		qWarning()<<"ERROR: key was invalid";
		return "";
	}
	else{
		QString sep(QDir::separator());
		QString dirPath;
		quint8 level=0;
		for(;level<DIR_LEVELS;++level){
			dirPath+=key[level]+sep;
		}
		QString filePath=key.right(key.size()-level);
		QString ret=m_dir.absoluteFilePath(dirPath+filePath);
		qDebug()<<"DIR: "<<m_dir.absolutePath()<<"PATH: "<<dirPath<<", FILE:" <<filePath<<", ABS:"<<ret;
		return ret;

	}
}
