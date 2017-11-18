#ifndef AGENTCLIENT_HPP
#define AGENTCLIENT_HPP

#include "Client.hpp"
#include "agent/AgentCourierSet.hpp"


#include <QtGlobal>

class AgentClientWidget;


/**
 * @brief The AgentClient is a specialization class derivig from Client that represents an agent that the current node has communications with
 */
class AgentClient : public Client
{
	Q_OBJECT
private:
	CourierSet mCouriers;
	AgentClientWidget *mWidget;
	// TODO: Figure out if this is complete horse manure or if it is actually sound (doubtful)
	QSharedPointer <AgentClient> mThis;
public:
	explicit AgentClient(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc, QObject *parent = nullptr);


	// Client interface
public:

	CourierSet &courierSet() Q_DECL_OVERRIDE;
	virtual QWidget *widget() Q_DECL_OVERRIDE;



};

#endif // AGENTCLIENT_HPP



