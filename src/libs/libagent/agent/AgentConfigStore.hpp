#ifndef AGENTCONFIGSTORE_HPP
#define AGENTCONFIGSTORE_HPP

#include "basic/AsyncStore.hpp"
#include "AgentConfig.hpp"


#include <QObject>
#include <QSharedPointer>

class PoseMapping;
/*

  AgentConfigStore is used to persist the physical configuration for agent.

  The store supports loading and saving asynchrounusly.


  TODO: Look at possability of overloading ConfigStore somehow
*/



class AgentConfigStore: public AsyncStore
{
	Q_OBJECT
private:

	QSharedPointer<AgentConfig> mAgentConfig;

public:
	explicit AgentConfigStore(QString filename="", QObject *parent = nullptr);
	virtual ~AgentConfigStore();

public:
	QSharedPointer<AgentConfig> agentConfig() const;
	void setAgentConfig(QSharedPointer<AgentConfig> config);

	// AsyncStore interface
protected:
	void bootstrapWorkerImpl() Q_DECL_OVERRIDE;
public:
	void load() Q_DECL_OVERRIDE;
	void save() Q_DECL_OVERRIDE;

signals:

	void agentConfigStoreReady(bool);
};

#endif // AGENTCONFIGSTORE_HPP
