#ifndef AGENTCONFIG_HPP
#define AGENTCONFIG_HPP

#include "../arduino/Actuator.hpp"

#include "PoseMapping.hpp"

#include <QVector>
#include <QSharedPointer>

class AgentConfig
{
private:
	QSharedPointer<PoseMapping> mPoseMapping;

	QVector<Actuator> mActuators;

public:
	AgentConfig();
	AgentConfig(const QVariantMap &map);


public:

	void fromMap(const QVariantMap &map);
	QVariantMap toMap();

	QSharedPointer<PoseMapping> poseMapping();
	void setPoseMapping(QSharedPointer<PoseMapping> mapping);

};

#endif // AGENTCONFIG_HPP
