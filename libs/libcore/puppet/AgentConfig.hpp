#ifndef AGENTCONFIG_HPP
#define AGENTCONFIG_HPP

#include "../arduino/ArduMYActuator.hpp"

#include "PoseMapping.hpp"

#include <QVector>
#include <QSharedPointer>

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

#endif // AGENTCONFIG_HPP
