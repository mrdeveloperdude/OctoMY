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
	virtual ~HubClient() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc);
	// Client interface
public:
	QSharedPointer<CourierSet> courierSet() Q_DECL_OVERRIDE;
	QSharedPointer<ClientWidget> widget() Q_DECL_OVERRIDE;

};

#endif
// HUBCLIENT_HPP
