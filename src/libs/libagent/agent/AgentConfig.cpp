#include "AgentConfig.hpp"

#include "uptime/MethodGate.hpp"
#include "stanza/StanzaBook.hpp"


AgentConfig::AgentConfig()
{
	OC_METHODGATE();
}


AgentConfig::AgentConfig(const QVariantMap &map)
{
	OC_METHODGATE();
	fromMap(map);
}


void AgentConfig::fromMap(const QVariantMap &map)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "fromMap" << map;
	}
	mControllerName = map["controllerName"].toString();
	mControllerConfig = map["controllerConfig"].toMap();
	mStanzaName = map["stanzaName"].toString();
	const auto stanzaList = map["stanzas"].toList();
	if(mStanzaBook.isNull()) {
		mStanzaBook = QSharedPointer<StanzaBook>::create();
	}
	mStanzaBook->fromList(stanzaList);
}


QVariantMap AgentConfig::toMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["controllerName"] = mControllerName;
	map["controllerConfig"] = mControllerConfig;
	map["stanzas"] = mStanzaBook.isNull()?QVariantList():mStanzaBook->toList();
	map["stanzaName"] = mStanzaName;
	if(mDebug){
		qDebug() << "toMap" << map;
	}
	return map;
}


QString AgentConfig::controllerName()
{
	OC_METHODGATE();
	return mControllerName;
}


void AgentConfig::setControllerName(const QString &n)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "setControllerName" << n;
	}
	mControllerName = n;
}



QString AgentConfig::stanzaName()
{
	OC_METHODGATE();
	return mStanzaName;
}


void AgentConfig::setStanzaName(const QString &n)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "setStanzaName" << n;
	}
	mStanzaName = n;
}


QVariantMap AgentConfig::controllerConfig()
{
	OC_METHODGATE();
	return mControllerConfig;
}


void AgentConfig::setControllerConfig(const QVariantMap &map)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "setControllerConfig" << map;
	}
	mControllerConfig = map;
}


QSharedPointer<StanzaBook> AgentConfig::stanzas()
{
	OC_METHODGATE();
	return mStanzaBook;
}


void AgentConfig::setStanzas(QSharedPointer<StanzaBook> stanzas)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "setStanzas" << stanzas;
	}
	mStanzaBook = stanzas;
}



QDebug operator<<(QDebug debug, const AgentConfig &config)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << "AgentConfig(\n"
					<< "\tControllerName: " << config.mControllerName << "\n"
					<< "\tControllerConfig: " << config.mControllerConfig << "\n"
					<< "\tStanzaName: " << config.mStanzaName << "\n"
					<< "\tStanzas: " << config.mStanzaBook << "\n"
					<< "\tDebug: " << (config.mDebug ? "true" : "false")<<"\n"
					<< ")\n";
	return debug;
}
