#include "AgentConfigStore.hpp"

#include "pose/PoseMapping.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QJsonParseError>
#include <QByteArray>

AgentConfigStore::AgentConfigStore(QString filename, QObject *parent)
	: QObject(parent)
	, SimpleDataStore(filename)
	, mAgentConfig(OC_NEW AgentConfig())
{
	OC_METHODGATE();
	setObjectName("AgentConfigStore");
}

AgentConfigStore::~AgentConfigStore()
{
	OC_METHODGATE();
	qDebug()<<"AGENT CONFIG STORE DTOR";
}


QSharedPointer<AgentConfig> AgentConfigStore::agentConfig() const
{
	OC_METHODGATE();
	return mAgentConfig;
}


void AgentConfigStore::setAgentConfig(QSharedPointer<AgentConfig> config)
{
	OC_METHODGATE();
	mAgentConfig=config;
}




bool AgentConfigStore::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	if(mAgentConfig.isNull()) {
		mAgentConfig=QSharedPointer<AgentConfig>(OC_NEW AgentConfig());
	}
	if(!mAgentConfig.isNull()) {
		mAgentConfig->fromMap(data);
		return true;
	}
	return false;
}

QVariantMap AgentConfigStore::toMap()
{
	OC_METHODGATE();
	QVariantMap map;
	if(!mAgentConfig.isNull()) {
		map=mAgentConfig->toMap();
	}
	return map;
}



bool AgentConfigStore::fromDefault()
{
	OC_METHODGATE();
	mAgentConfig=QSharedPointer<AgentConfig>(OC_NEW AgentConfig());
	return true;
}
