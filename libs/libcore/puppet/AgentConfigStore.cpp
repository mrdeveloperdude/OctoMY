#include "AgentConfigStore.hpp"

#include "puppet/PoseMapping.hpp"
#include "../libutil/utility/Standard.hpp"
#include "../libutil/utility/Utility.hpp"

#include <QJsonParseError>
#include <QByteArray>

AgentConfigStore::AgentConfigStore(QString filename, QObject *parent)
	: AsyncStore(filename, parent)
	, mAgentConfig(new AgentConfig())
{
	setObjectName("AgentConfigStore");
	//qDebug()<<"PoseMappingStore() file="<<fn;
	// Forward the async storeReady signal
	if(!connect( this, SIGNAL(storeReady(bool)), SIGNAL(poseMappingStoreReady(bool)), OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<objectName();
	} else {
		//qDebug()<<"FORWARDING storeReady -> peerStoreReady";
	}
}

AgentConfigStore::~AgentConfigStore()
{
	save();
}


QSharedPointer<AgentConfig> AgentConfigStore::agentConfig() const
{
	return mAgentConfig;
}


void AgentConfigStore::setAgentConfig(QSharedPointer<AgentConfig> config)
{
	mAgentConfig=config;
}

// AsyncStore interface
////////////////////////////

void AgentConfigStore::bootstrapWorkerImpl()
{
	//qDebug()<<"PoseMappingStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()) {
		qDebug()<<"PoseMappingStore: no keystore file found, saving";
		save();
	}
	load();
}


void AgentConfigStore::load()
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
			QVariantMap agentConfigMap=map["agentConfig"].toMap();
			if(mAgentConfig.isNull()) {
				mAgentConfig=QSharedPointer<AgentConfig>(new AgentConfig());
			}
			if(!mAgentConfig.isNull()) {
				mAgentConfig->fromMap(agentConfigMap);
			}
			mReady=true;
		}
	}
	//qDebug()<<"EMITTING storeReady";
	emit poseMappingStoreReady(!mError);
}

void AgentConfigStore::save()
{
	qDebug()<<"AgentConfigStore: Saving to file: "<<mFilename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	if(!mAgentConfig.isNull()) {
		map["agentConfig"]=mAgentConfig->toMap();
	}
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	utility::stringToFile(mFilename,doc.toJson());
}
