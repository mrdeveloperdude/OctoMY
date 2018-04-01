#ifndef AGENTCLIENT_HPP
#define AGENTCLIENT_HPP

#include "Client.hpp"
#include "comms/couriers/sets/AgentClientCourierSet.hpp"


#include <QtGlobal>

class AgentClientWidget;


/**
 * @brief The AgentClient is a specialization class derivig from Client that represents an agent that the current node has communications with
 */
class AgentClient : public Client//, public QEnableSharedFromThis<AgentClient>  <-- DON'T use this, qSharedPointerCast<AgentClient>( QenableSharedFromThis<Client>::sharedFromThis() instead )
{
	Q_OBJECT
private:
	AgentClientCourierSet mCouriers;
	AgentClientWidget *mWidget;

public:
	explicit AgentClient(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc, QObject *parent = nullptr);


	// Client interface
public:
	CourierSet &courierSet() Q_DECL_OVERRIDE;
	ClientWidget *widget() Q_DECL_OVERRIDE;



};

#endif // AGENTCLIENT_HPP



