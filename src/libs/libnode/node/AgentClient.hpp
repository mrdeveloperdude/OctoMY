#ifndef REMOTECLIENT_HPP
#define REMOTECLIENT_HPP

#include "Client.hpp"
#include "RemoteCourierSet.hpp"


#include <QtGlobal>

class RemoteClientWidget;

class RemoteClient : public Client
{
	Q_OBJECT
private:
	RemoteCourierSet mCouriers;
	RemoteClientWidget *mWidget;
	// TODO: Figure out if this is complete horse manure or if it is actually sound (doubtful)
	QSharedPointer <RemoteClient> mThis;
public:
	explicit RemoteClient(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc, QObject *parent = nullptr);


	// Client interface
public:

	CourierSet &courierSet() Q_DECL_OVERRIDE;
	virtual QWidget *widget() Q_DECL_OVERRIDE;



};

#endif // REMOTECLIENT_HPP
