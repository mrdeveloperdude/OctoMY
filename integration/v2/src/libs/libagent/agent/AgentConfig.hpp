#ifndef AGENTCONFIG_HPP
#define AGENTCONFIG_HPP

//#include "ardumy_arduino/ArduMYActuator.hpp"

#include "pose/PoseMapping.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QVariantList>
#include <QVariantMap>


class AgentConfig
{
private:
	QString mControllerName;
	QVariantMap mControllerConfig;
	QSharedPointer<PoseMapping> mPoseMapping;

public:
	AgentConfig();
	AgentConfig(const QVariantMap &map);

public:
	void fromMap(const QVariantMap &map);
	QVariantMap toMap();

public:
	QString controllerName();
	void setControllerName(const QString &);

	QVariantMap controllerConfig();
	void setControllerConfig(const QVariantMap &);

	QSharedPointer<PoseMapping> poseMapping();
	void setPoseMapping(QSharedPointer<PoseMapping> mapping);

};

#endif
// AGENTCONFIG_HPP
