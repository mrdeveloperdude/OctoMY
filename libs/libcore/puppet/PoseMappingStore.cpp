#include "PoseMappingStore.hpp"

#include "../libutil/utility/Standard.hpp"
#include "../libutil/utility/Utility.hpp"
#include <QJsonParseError>
#include <QByteArray>

PoseMappingStore::PoseMappingStore(QObject *parent)
	: AsyncStore("PoseMapping", parent)
	, mPoseMapping(new PoseMapping(0))
{
	setObjectName("PoseMappingStore");
	//qDebug()<<"PoseMappingStore() file="<<fn;
	// Forward the async storeReady signal
	if(!connect( this, SIGNAL(storeReady(bool)), SIGNAL(poseMappingStoreReady(bool)), OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<objectName();
	} else {
		//qDebug()<<"FORWARDING storeReady -> peerStoreReady";
	}
}

PoseMappingStore::~PoseMappingStore()
{
	save();
}


QSharedPointer<PoseMapping> PoseMappingStore::poseMapping()
{
	return mPoseMapping;
}


void PoseMappingStore::setPoseMapping(QSharedPointer<PoseMapping> mapping)
{
	mPoseMapping=mapping;
}

// AsyncStore interface
////////////////////////////

void PoseMappingStore::bootstrapWorkerImpl()
{
	//qDebug()<<"PoseMappingStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()) {
		qDebug()<<"PoseMappingStore: no keystore file found, saving";
		save();
	}
	load();
}


void PoseMappingStore::load()
{
	//qDebug()<<"NodeAssociateStore: Loading from file "<<mFilename;
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
			QVariantList list=map["poseMapping"].toList();
			if(mPoseMapping.isNull()) {
				mPoseMapping=QSharedPointer<PoseMapping>(new PoseMapping(list.size()));
			}
			mPoseMapping->fromMap(list);
			mReady=true;
		}
	}
	//qDebug()<<"EMITTING storeReady";
	emit poseMappingStoreReady(!mError);
}

void PoseMappingStore::save()
{
	//qDebug()<<"NodeAssociateStore: Saving to file: "<<mFilename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	if(!mPoseMapping.isNull()) {
		map["poseMapping"]=mPoseMapping->toMap();
	}
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	utility::stringToFile(mFilename,doc.toJson());
}
