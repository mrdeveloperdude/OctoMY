#ifndef HUBCLIENT_HPP
#define HUBCLIENT_HPP

#include "Client.hpp"
#include "HubCourierSet.hpp"


#include <QtGlobal>

class HubClientWidget;

/**
 * @brief The HubClient is a specialization class derivig from Client that represents a hub that the current node has communications with
 */

class HubClient : public Client, public QEnableSharedFromThis<HubClient>
{
	Q_OBJECT
private:
	CourierSet mCouriers;
	HubClientWidget *mWidget;

public:
	explicit HubClient(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc, QObject *parent = nullptr);


	// Client interface
public:

	CourierSet &courierSet() Q_DECL_OVERRIDE;
	virtual QWidget *widget() Q_DECL_OVERRIDE;



};

#endif // HUBCLIENT_HPP