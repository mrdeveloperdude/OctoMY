#ifndef CLIENT_HPP
#define CLIENT_HPP



#include "comms/couriers/CourierSet.hpp"

#include "basic/Associate.hpp"



#include <QSharedPointer>
#include <QObject>
#include <QTimer>

/**
 * @brief The Client class represents the state of an associate at runtime. It is what enables many-to-many in comms .
 */

class Node;
class CommsSession;
class CourierSet;
class ClientConnectionStatus;
class ClientWidget;
class CommsChannel;

class ISyncParameter;





class Client: public QObject, public QEnableSharedFromThis<Client>
{
	Q_OBJECT
protected:

	QTimer mUpdateTimer;
	QSharedPointer<Node> mNode;
	QSharedPointer<Associate> mAssociate;

public:

	explicit Client(QSharedPointer<Node> node, QSharedPointer<Associate> associate, QObject *parent=nullptr);
	virtual ~Client();


// Client interface
public:
	virtual CourierSet &courierSet() =0;
	virtual ClientWidget *widget() =0;



protected:

	friend class ClientConnectionStatus;
	friend class AgentClientWidget;

	bool needsConnection();
	bool isConnected();
	void setNeedsConnection(const bool);
	void setConnected(const bool);

public:

	ClientConnectionStatus connectionStatus();

public:

	void initTimer();

	QSharedPointer<Node> node();
	CommsChannel *comms();
	QSharedPointer<Associate> associate();

	bool courierRegistration();
	void setCourierRegistration(bool reg);
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

#endif // CLIENT_HPP
