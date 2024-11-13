#ifndef AGENTCONFIG_HPP
#define AGENTCONFIG_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QVariantList>
#include <QVariantMap>


class StanzaBook;
class PoseMapping;
	
class AgentConfig
{
private:
	QString mControllerName;
	QString mStanzaName;
	QVariantMap mControllerConfig;
	QVariantMap mLobeConfig;
	QSharedPointer<StanzaBook> mStanzaBook;
	
	bool mDebug{false};

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

	QString stanzaName();
	void setStanzaName(const QString &);

	QSharedPointer<StanzaBook> stanzas();
	void setStanzas(QSharedPointer<StanzaBook> stanzas);
	
public:
	friend QDebug operator<<(QDebug debug, const AgentConfig &config);
};

#endif
// AGENTCONFIG_HPP
