#include "AgentConfigStore.hpp"

//#include "pose/PoseMapping.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QJsonParseError>
#include <QByteArray>

AgentConfigStore::AgentConfigStore(QObject *parent)
	: QObject(parent)
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


void AgentConfigStore::configure(QString filename)
{
	OC_METHODGATE();
	SimpleDataStore::configure(filename);
}

QSharedPointer<AgentConfig> AgentConfigStore::agentConfig() const
{
	OC_METHODGATE();
	return mAgentConfig;
}


void AgentConfigStore::setAgentConfig(QSharedPointer<AgentConfig> config)
{
	OC_METHODGATE();
	mAgentConfig = config;
}


bool AgentConfigStore::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	if(mAgentConfig.isNull()) {
		mAgentConfig = QSharedPointer<AgentConfig>::create();
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
		map = mAgentConfig->toMap();
	}
	return map;
}


bool AgentConfigStore::fromDefault()
{
	OC_METHODGATE();
	mAgentConfig = QSharedPointer<AgentConfig>::create();
	return true;
}
