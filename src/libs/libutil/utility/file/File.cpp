#include "utility/file/File.hpp"


#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QJsonParseError>

#include <QMediaContent>



namespace utility
{


namespace file
{


QString fileToString(QString fn)
{
	QFile file(fn);
	if(file.open(QFile::ReadOnly)) {
		QTextStream in(&file);
		return in.readAll();
	}
	return "";
}


QByteArray fileToByteArray(QString fn)
{
	QFile file(fn);
	if(file.open(QFile::ReadOnly)) {
		return file.readAll();
	} else {
		qWarning().nospace().noquote()<<"ERROR: Could not open "<<fn << " for reading: "<< file.errorString();
	}
	return "";
}


QVariant jsonFileToVariant(QString fn)
{
	QJsonParseError jsonError;
	QByteArray raw=fileToByteArray(fn);
	if(raw.size()>0) {
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<fn;
		} else {
			//qDebug()<<"PARSED JSON: "<<doc.toJson();
			return doc.toVariant();
		}
	}
	return QVariant();
}

bool ensureDirExistsForFile(QString fn)
{
	QFile file(fn);
	QDir dir = QFileInfo(file).absoluteDir();
	if(!dir.exists()) {
		return dir.mkpath(dir.absolutePath());
	}
	return true;
}


bool variantToJsonFile(QString fn, QVariant data)
{
	QJsonDocument doc=QJsonDocument::fromVariant(data);
	return stringToFile(fn, doc.toJson());
}




bool stringToFile(QString fn, QString data, bool append)
{
	QFile file(fn);
	if(file.open(append?(QFile::WriteOnly|QFile::Append):(QFile::WriteOnly))) {
		QTextStream out(&file);
		out<<data;
		out.flush();
		file.flush();
		file.close();
		// sync();
		return true;
	}
	return false;
}



bool byteArrayToFile(QString fn, QByteArray data, bool append)
{
	QFile file(fn);
	if(file.open(append?(QFile::WriteOnly|QFile::Append):(QFile::WriteOnly))) {
		const qint64 total=data.size();
		qint64 acc=0;
		while(acc<total) {
			qint64 written=file.write(data.data(),total-acc);
			if(written<0) {
				return false;
			}
			//qDebug()<<"WRITE LOOP total="<<total<<", acc="<<acc<<", written="<<written;
			acc+=written;
		}
		file.flush();
		file.close();
		// sync();
		return true;
	}
	return false;
}

bool verifyMedia(QString name)
{
	QString msg="";
	bool ret=false;
	if(""==name.trimmed()) {
		msg="content path empty";
	} else {
		QFileInfo file(name);
		for(int i=0; i<10 && file.isSymLink(); i++) {
			file=QFileInfo(file.symLinkTarget());
		}
		if(!file.exists()) {
			msg="media did not exist";
		} else if(!file.isFile()) {
			msg="media is not a regular file";
		} else if(!file.isReadable()) {
			msg="media not readable";
		} else {
			QMediaContent mc(name);
			if(mc.isNull()) {
				msg="media was null";
			} else {
				ret=true;
			}
		}
	}
	if(!ret) {
		qDebug()<<"VERIFY CONTENT FAILED: "<<msg;
	}
	return ret;
}



}
}
