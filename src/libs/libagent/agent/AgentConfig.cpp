#include "AgentConfig.hpp"
#include "uptime/MethodGate.hpp"


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
	const auto list = map["poseMapping"].toList();
	if(mPoseMapping.isNull()) {
		mPoseMapping = QSharedPointer<PoseMapping>::create(static_cast<quint32>(list.size()));
	}
	mPoseMapping->fromMap(list);
}


QVariantMap AgentConfig::toMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["controllerName"] = mControllerName;
	map["controllerConfig"] = mControllerConfig;
	map["poseMapping"] = mPoseMapping.isNull()? QVariantList(): mPoseMapping->toMap();
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


QSharedPointer<PoseMapping> AgentConfig::poseMapping()
{
	OC_METHODGATE();
	return mPoseMapping;
}


void AgentConfig::setPoseMapping(QSharedPointer<PoseMapping> mapping)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "setPoseMapping" << mapping;
	}
	mPoseMapping = mapping;
}



QDebug operator<<(QDebug debug, const AgentConfig &config)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << "AgentConfig(\n"
					<< "\tControllerName: " << config.mControllerName << "\n"
					<< "\tControllerConfig: " << config.mControllerConfig << "\n"
					<< "\tPoseMapping: " << (config.mPoseMapping ? "Valid" : "Null") << "\n"
					<< "\tDebug: " << (config.mDebug ? "true" : "false")<<"\n"
					<< ")\n";
	return debug;
}
