#ifndef REMOTECLIENT_HPP
#define REMOTECLIENT_HPP

#include "Client.hpp"
#include "RemoteCourierSet.hpp"


#include <QtGlobal>

class RemoteClientWidget;

/**
 * @brief The RemoteClient is a specialization class derivig from Client that represents a remote that the current node has communications with
 */
class RemoteClient : public Client//, public QEnableSharedFromThis<RemoteClient> <-- DON'T use this, qSharedPointerCast<RemoteClient>( QenableSharedFromThis<Client>::sharedFromThis() instead )
{
	Q_OBJECT
private:
	RemoteCourierSet mCouriers;
	RemoteClientWidget *mWidget;

public:
	explicit RemoteClient(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc, QObject *parent = nullptr);
	virtual ~RemoteClient();


	// Client interface
public:

	CourierSet &courierSet() Q_DECL_OVERRIDE;
	ClientWidget *widget() Q_DECL_OVERRIDE;



};

#endif // REMOTECLIENT_HPP
