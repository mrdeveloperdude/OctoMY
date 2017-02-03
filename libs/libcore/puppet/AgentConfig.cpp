#include "AgentConfig.hpp"
#include "PoseMapping.hpp"

#include <QSharedPointer>

AgentConfig::AgentConfig()
{

}

AgentConfig::AgentConfig(const QVariantMap &map)
{
	fromMap(map);

}


void AgentConfig::fromMap(const QVariantMap &map)
{
	mControllerName=map["controllerName"].toString();
	mControllerConfig=map["controllerConfig"].toMap();
	QVariantList list=map["poseMapping"].toList();
	if(mPoseMapping.isNull()) {
		mPoseMapping=QSharedPointer<PoseMapping>(new PoseMapping(list.size()));
	}
	mPoseMapping->fromMap(list);
}

QVariantMap AgentConfig::toMap()
{
	QVariantMap map;
	map["controllerName"]=mControllerName;


	map["controllerConfig"]=mControllerConfig;
	QVariantList list=map["poseMapping"].toList();
	if(mPoseMapping.isNull()) {
		mPoseMapping=QSharedPointer<PoseMapping>(new PoseMapping(list.size()));
	}
	mPoseMapping->fromMap(list);



	// TODO: Implement
	return map;
}





QString AgentConfig::controllerName()
{
	return mControllerName;
}

void AgentConfig::setControllerName(const QString &n)
{
	mControllerName=n;
}

QVariantMap AgentConfig::controllerConfig()
{
	return mControllerConfig;
}

void AgentConfig::setControllerConfig(const QVariantMap &map)
{
	mControllerConfig=map;
}



QSharedPointer<PoseMapping> AgentConfig::poseMapping()
{
	return mPoseMapping;
}


void AgentConfig::setPoseMapping(QSharedPointer<PoseMapping> mapping)
{
	mPoseMapping=mapping;
}
