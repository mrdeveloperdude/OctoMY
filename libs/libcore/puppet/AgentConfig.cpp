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
	QVariantList list=map["poseMapping"].toList();
	if(mPoseMapping.isNull()) {
		mPoseMapping=QSharedPointer<PoseMapping>(new PoseMapping(list.size()));
	}
	mPoseMapping->fromMap(list);

}

QVariantMap AgentConfig::toMap()
{
	QVariantMap map;
	return map;
}

QSharedPointer<PoseMapping> AgentConfig::poseMapping()
{
	return mPoseMapping;
}


void AgentConfig::setPoseMapping(QSharedPointer<PoseMapping> mapping)
{
	mPoseMapping=mapping;
}
