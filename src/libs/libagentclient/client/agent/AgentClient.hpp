#ifndef AGENTCLIENT_HPP
#define AGENTCLIENT_HPP

#include "client/Client.hpp"
#include "comms/couriers/sets/AgentClientCourierSet.hpp"


#include <QtGlobal>

class AgentClientWidget;


/**
 * @brief The AgentClient is a specialization class derived from Client that represents an agent that the current node has communications with
 */
class AgentClient : public Client//, public QEnableSharedFromThis<AgentClient>  <-- DON'T use this, use qSharedPointerCast<AgentClient>( QenableSharedFromThis<Client>::sharedFromThis() instead )
{
	Q_OBJECT
private:
	QSharedPointer<AgentClientCourierSet> mCouriers;
	QSharedPointer<ClientWidget> mWidget;

public:
	explicit AgentClient(QObject *parent = nullptr);
	virtual ~AgentClient() override;

public:
	void configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc);


	// Client interface
public:
	QSharedPointer<CourierSet> courierSet() override;
	QSharedPointer<ClientWidget> widget() override;



};

#endif // AGENTCLIENT_HPP



