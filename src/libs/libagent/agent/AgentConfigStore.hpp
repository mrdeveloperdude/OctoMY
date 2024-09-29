#ifndef AGENTCONFIGSTORE_HPP
#define AGENTCONFIGSTORE_HPP

#include "store/SimpleDataStore.hpp"
#include "AgentConfig.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QObject>

/** The AgentConfigStore class is used to persist the physical configuration for agent.
  The store supports loading and saving asynchrounusly.
  TODO: Look at possability of overloading ConfigStore somehow

*/


class AgentConfigStore: public QObject, public SimpleDataStore
{
	Q_OBJECT

private:
	QSharedPointer<AgentConfig> mAgentConfig;

public:
	explicit AgentConfigStore(QObject *parent = nullptr);
	virtual ~AgentConfigStore() override;

public:
	void configure(QString filename="");

public:
	QSharedPointer<AgentConfig> agentConfig() const;
	void setAgentConfig(QSharedPointer<AgentConfig> config);

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data)  override;
	QVariantMap toMap() override;
	bool fromDefault()  override;

signals:
	void agentConfigStoreReady(bool);
};

#endif
// AGENTCONFIGSTORE_HPP

