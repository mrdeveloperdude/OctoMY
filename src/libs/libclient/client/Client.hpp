#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "comms/couriers/CourierSet.hpp"

#include "address/Associate.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QObject>
#include <QTimer>

class ClientConnectionStatus;
class ClientWidget;
class Comms;
class CommsChannel;
class CommsSession;
class CourierSet;
class ISyncParameter;
class Node;


/**
 * @brief The Client class is the base class for clients, which are running instances
 * representing nodes on the other side of comms channel.
 * Clients represent the state of an associate at runtime. It is what enables
 * many-to-many in comms.
 * 
 * Clients are instanciated from associate that identifies the node on the other side
 * and maintain state relevant to that node.
 * 
 * Sub-classes exist in AgentClient, RemoteClient and HubClient that specializes
 * the class to spesific node types.
 */


class Client: public QObject, public QEnableSharedFromThis<Client>
{
	Q_OBJECT

protected:
	QTimer mUpdateTimer;
	QSharedPointer<Node> mNode;
	QSharedPointer<Associate> mAssociate;

protected:
	friend class ClientConnectionStatus;
	friend class AgentClientWidget;

public:
	explicit Client(QObject *parent = nullptr);
	virtual ~Client();

public:
	void configure(QSharedPointer<Node> node, QSharedPointer<Associate> associate);

// Client interface
public:
	virtual QSharedPointer<CourierSet> courierSet() =0;
	virtual QSharedPointer<ClientWidget> widget() =0;

protected:
	bool needsConnection();
	bool isConnected();
	void setNeedsConnection(const bool);
	void setConnected(const bool);

public:
	ClientConnectionStatus connectionStatus();

public:
	void initTimer();
	QSharedPointer<Node> node();
	QSharedPointer<Comms> comms();
	QSharedPointer<Associate> associate();
	bool courierRegistration();
	void registerCouriers(bool reg);
	void updateCourierRegistration();
	void updateOnlineStatus();


	// Agent State Courier slots
public slots:
	void onSyncParameterChanged(ISyncParameter *);

	// CommsChannel slots
private slots:
	void onCommsError(QString);
	void onCommsClientAdded(CommsSession *);
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);

	// Internal slots
public slots:
	void onUpdateTimer();
	void appendLog(const QString& text);

};

#endif
// CLIENT_HPP
