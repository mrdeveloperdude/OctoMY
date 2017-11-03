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

class Client: public QObject
{
	Q_OBJECT
protected:

	QTimer mUpdateTimer;
	QSharedPointer<Node> mNode;
	QSharedPointer<Associate> mNodeAssociate;



public:

	explicit Client(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc, QObject *parent=nullptr);
	virtual ~Client();


// Client interface
public:

	virtual CourierSet &courierSet() =0;

	virtual QWidget *widget() =0;


public:

	void initTimer();

	QSharedPointer<Node> node();
	QSharedPointer<Associate> nodeAssociate();

	bool courierRegistration();
	void setCourierRegistration(bool reg);


	void updateOnlineStatus();



	// Agent State Courier slots
public slots:
	void onSyncParameterChanged(ISyncParameter *);

	// CommsChannel slots
private slots:
	void onCommsError(QString);
	void onCommsClientAdded(CommsSession *);
	void onCommsConnectionStatusChanged(bool);



	// Internal slots
public slots:
	void onUpdateTimer();
	void appendLog(const QString& text);




};

#endif // CLIENT_HPP
