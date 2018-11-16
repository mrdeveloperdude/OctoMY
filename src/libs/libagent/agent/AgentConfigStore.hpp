#ifndef AGENTCONFIGSTORE_HPP
#define AGENTCONFIGSTORE_HPP

#include "node/SimpleDataStore.hpp"
#include "AgentConfig.hpp"


#include <QObject>
#include <QSharedPointer>

class PoseMapping;
/*

  AgentConfigStore is used to persist the physical configuration for agent.

  The store supports loading and saving asynchrounusly.

  TODO: Look at possability of overloading ConfigStore somehow

*/



class AgentConfigStore: public QObject, public SimpleDataStore
{
	Q_OBJECT
private:
	QSharedPointer<AgentConfig> mAgentConfig;

public:
	explicit AgentConfigStore(QString filename="", QObject *parent = nullptr);
	virtual ~AgentConfigStore() Q_DECL_OVERRIDE;

public:
	QSharedPointer<AgentConfig> agentConfig() const;
	void setAgentConfig(QSharedPointer<AgentConfig> config);


	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data)  Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;
	bool fromDefault()  Q_DECL_OVERRIDE;




signals:

	void agentConfigStoreReady(bool);
};

#endif // AGENTCONFIGSTORE_HPP
