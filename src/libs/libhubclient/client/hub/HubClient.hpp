#ifndef HUBCLIENT_HPP
#define HUBCLIENT_HPP

#include "client/Client.hpp"
#include "comms/couriers/sets/HubClientCourierSet.hpp"

#include <QtGlobal>

class HubClientWidget;

/**
 * @brief The HubClient is a specialization class derivig from Client that represents a hub that the current node has communications with
 */

class HubClient : public Client//, public QEnableSharedFromThis<HubClient>  <-- DON'T use this, qSharedPointerCast<HubClient>( QenableSharedFromThis<Client>::sharedFromThis() instead )
{
	Q_OBJECT
private:
	QSharedPointer<HubClientCourierSet> mCouriers;
	QSharedPointer<HubClientWidget> mWidget;

public:
	explicit HubClient(QObject *parent = nullptr);
	virtual ~HubClient() override;

public:
	void configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc);
	// Client interface
public:
	QSharedPointer<CourierSet> courierSet() override;
	QSharedPointer<ClientWidget> widget() override;

};

#endif
// HUBCLIENT_HPP
