#ifndef CONNECTIONSTATUSWIDGET_HPP
#define CONNECTIONSTATUSWIDGET_HPP

#include <QWidget>


#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class CommsSession;
class Node;

namespace Ui {
class ConnectionStatusWidget;
}

class ConnectionStatusWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ConnectionStatusWidget *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	
	
public:
	explicit ConnectionStatusWidget(QWidget *parent = nullptr);
	~ConnectionStatusWidget();
	
private:
	void appendLog(const QString &text);

public:
	bool connecting() const;
	void configure(QSharedPointer<Node>);
	void setPanic(bool panic);

private slots:
	void connectToggled(bool connect);
	void openSettings();
	
	// CommsChannel slots (via setHookCommsSignals in AgentWindow)
public slots:
	// An error occurred in comms
	void onCommsError(QString message);
	// A new comms session was added
	void onCommsClientAdded(CommsSession *c);
	// The connection state changed for comms channel
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);


private slots:
	void randomizeColor();
	void panicChanged(bool panic);

signals:
	bool tryConnect(bool connect);
	bool openConnectionSettings();
	void colorChanged(QColor color);
};

#endif // CONNECTIONSTATUSWIDGET_HPP
