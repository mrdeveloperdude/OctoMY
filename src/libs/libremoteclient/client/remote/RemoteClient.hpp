#ifndef REMOTECLIENT_HPP
#define REMOTECLIENT_HPP

#include "client/Client.hpp"
#include "comms/couriers/sets/RemoteCourierSet.hpp"


#include <QtGlobal>

class RemoteClientWidget;

/**
 * @brief The RemoteClient is a specialization class derivig from Client that
 * represents a remote that the current node has communications with
 */
class RemoteClient : public Client//, public QEnableSharedFromThis<RemoteClient> <-- DON'T use this, qSharedPointerCast<RemoteClient>( QenableSharedFromThis<Client>::sharedFromThis() instead )
{
	Q_OBJECT
private:
	QSharedPointer<RemoteCourierSet> mCouriers;
	QSharedPointer<RemoteClientWidget> mWidget;

public:
	explicit RemoteClient(QObject *parent = nullptr);
	virtual ~RemoteClient() override;

public:
	void configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc);

	// Client interface
public:
	QSharedPointer<CourierSet> courierSet() override;
	QSharedPointer<ClientWidget> widget() override;


};

#endif
// REMOTECLIENT_HPP
