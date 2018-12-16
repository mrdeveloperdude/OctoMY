#include "JsonAsyncBackend.hpp"

JsonAsyncBackend::JsonAsyncBackend(QString filename)
	: mFilename(filename)
{

}

JsonAsyncBackend::~JsonAsyncBackend()
{

}

void JsonAsyncBackend::setFilenameBackend(QString filename){
	OC_METHODGATE();
	mFilename=filename;
}


QString JsonAsyncBackend::filenameBackend() {
	OC_METHODGATE();
	return mFilename;
}

bool JsonAsyncBackend::existsBackend()
{
	OC_METHODGATE();
	return QFile(mFilename).exists();
}


bool JsonAsyncBackend::clearBackend()
{
	OC_METHODGATE();
	QFile file(mFilename);
	bool fileOk=false;
	if(file.exists()) {
		if(file.remove()) {
			fileOk=true;
		} else {
			qWarning().noquote().nospace()<<"ERROR: Could not clear file '"<<mFilename<<"'";
		}
	} else {
		fileOk=true;
	}
	return fileOk;
}


bool JsonAsyncBackend::saveBackend(QVariantMap data)
{
	OC_METHODGATE();
	//qDebug()<<"Entering saveBackend";
	bool succeeded=false;
	//qDebug().noquote().nospace()<<"Saving data to file '"<<mFilename<<"'";
	const auto t=QDateTime::currentDateTime();
	if(!data.contains("createdTimeStamp")) {
		data["createdTimeStamp"]=t;
	}
	data["modifiedTimeStamp"]=t;
	QJsonDocument doc=QJsonDocument::fromVariant(data);
	succeeded=utility::stringToFile(mFilename, doc.toJson());
	//qDebug()<<"Exiting Sync Save with succeeded="<<succeeded;
	return succeeded;
}


QVariantMap JsonAsyncBackend::loadBackend(bool &ok)
{
	OC_METHODGATE();
	//qDebug()<<"Entering loadBackend";'
	QVariantMap data;
	ok=false;
	//qDebug().noquote().nospace()<<"Loading data from file '"<<mFilename<<"'";

	// TODO: Will this ever occur?
	if(!existsBackend()){
		qWarning()<<"ERROR: NO Backend while loading json";
		return data;
	}

	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(mFilename);
	if(raw.size()<=0) {
		//Let empty data pass because it is valid the first time app starts
		qWarning().nospace().noquote() << "WARNING: Data read from file '"<<mFilename<< "' was empty";
		ok=true;
	} else {
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning().nospace().noquote() << "ERROR: Parsing json data "<<raw<<" from file '"<<mFilename<<"' produced error '"<<jsonError.errorString()<<"'";
			ok=false;
		} else {
			data=doc.object().toVariantMap();
			ok=true;
		}
	}
	return data;
}


