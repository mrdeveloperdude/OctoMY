#include "ConfigStore.hpp"


#include "pose/PoseMapping.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QJsonParseError>
#include <QByteArray>

ConfigStore::ConfigStore(QString filename, QObject *parent)
	: AsyncStore(filename, parent)
{
	OC_METHODGATE();
	setObjectName("ConfigStore");
	//qDebug()<<"PoseMappingStore() file="<<fn;
	// Forward the async storeReady signal
	if(!connect( this, SIGNAL(storeReady(bool)), SIGNAL(configStoreReady(bool)), OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<objectName();
	} else {
		//qDebug()<<"FORWARDING storeReady -> peerStoreReady";
	}
}

ConfigStore::~ConfigStore()
{
	OC_METHODGATE();
	qDebug()<<"CONFIG STORE DTOR";
	save();
}



QVariantMap ConfigStore::config() const
{
	OC_METHODGATE();
	return mConfig;
}


void ConfigStore::setConfig(QVariantMap config)
{
	OC_METHODGATE();
	mConfig=config;
}




void ConfigStore::setHookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if(hook) {
		if(!connect(this, SIGNAL(configStoreReady(bool)), &ob, SLOT(onConfigReady(bool)),OC_CONTYPE)) {
			qWarning()<<"Could not connect "<<ob.objectName();
		} else {
			//qDebug()<<"HOOKING configStoreReady";
		}
	} else {
		if(!disconnect(this, SIGNAL(configStoreReady(bool)), &ob, SLOT(onConfigReady(bool)))) {
			qWarning()<<"Could not disconnect "<<ob.objectName();
		} else {
			//qDebug()<<"UN-HOOKING configStoreReady";
		}
	}

}




void ConfigStore::clear()
{
	OC_METHODGATE();
	QFile file(mFilename);
	if(file.exists()) {
		if(file.remove()) {
			//qDebug()<<"KEYSTORE: Cleared: "<<*this;
			mConfig.clear();
			mReady=false;
			mError=false;
		} else {
			qWarning()<<"ERROR: Could not clear "<<*this;
		}
	} else {
		//qDebug()<<"KEYSTORE: Could not clear missing file: "<< *this;
	}
}


// AsyncStore interface
////////////////////////////

void ConfigStore::bootstrapWorkerImpl()
{
	OC_METHODGATE();
	//qDebug()<<"PoseMappingStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()) {
		qDebug()<<"PoseMappingStore: no keystore file found, saving";
		save();
	}
	load();
}


void ConfigStore::load()
{
	OC_METHODGATE();
	qDebug()<<"ConfigStore: Loading from file: "<<mFilename;
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(mFilename);
	if(raw.size()<1) {
		//Let empty data pass because it is valid the first time app starts
		qWarning() << "WARNING: Data read from file "<<mFilename<< " was empty";
		mReady=true;
	} else {
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<mFilename;
			mError=true;
		} else {
			//qDebug()<<"PARSED JSON: "<<doc.toJson();
			QVariantMap map = doc.object().toVariantMap();
			mConfig=map["config"].toMap();
			mReady=true;
		}
	}
	//qDebug()<<"EMITTING storeReady";
	emit configStoreReady(!mError);
}

void ConfigStore::save()
{
	OC_METHODGATE();
	qDebug()<<"ConfigStore: Saving to file: "<<mFilename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	map["config"]=mConfig;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	utility::stringToFile(mFilename,doc.toJson());
}



const QDebug &operator<<(QDebug &d, ConfigStore &ks)
{
	OC_FUNCTIONGATE();
	d.nospace() <<"ConfigStore{ fn="<<ks.mFilename<<", fexists="<<ks.fileExists()<<", ready="<<(const bool)ks.mReady<<", inProgress="<<(const bool)ks.mInProgress<<", error="<<(const bool)ks.mError<<", config:{";
	d.nospace() << ks.mConfig;
	d.nospace() <<"}}";
	return d.maybeSpace();
}
