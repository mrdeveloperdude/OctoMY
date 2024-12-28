#ifndef CONNECTIONACTIVITY_HPP
#define CONNECTIONACTIVITY_HPP

#include "app/log/LogDestination.hpp"
#include "components/TryToggleState.hpp"
#include "components/navigation/Activity.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>

class Node;
class CommsSession;

namespace Ui {
class ConnectionActivity;
}

class ConnectionActivity : public Activity, public LogDestination
{
	Q_OBJECT
private:
	Ui::ConnectionActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit ConnectionActivity(QWidget *parent = nullptr);
	~ConnectionActivity();

public:
	void configure(QSharedPointer<Node> node);
	
	// Connection
private slots:
	void tryConnectChanged(const TryToggleState last, const TryToggleState current);
	
	
	// LogDestination interface
public:
	void appendLog(const QString& text) override;

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	
	// CommsChannel slots (via setHookCommsSignals in AgentWindow)
public slots:
	// An error occurred in comms
	void onCommsError(QString message);
	// A new comms session was added
	void onCommsClientAdded(CommsSession *c);
	// The connection state changed for comms channel
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);
};

#endif // CONNECTIONACTIVITY_HPP
