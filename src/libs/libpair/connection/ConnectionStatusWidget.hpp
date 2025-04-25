#ifndef CONNECTIONSTATUSWIDGET_HPP
#define CONNECTIONSTATUSWIDGET_HPP

#include "log/LogType.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>

class CommsSession;
struct DiscoveryMandate;
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
	void log(const QString &text, LogType type=LOG_TYPE_INFO) const;

public:
	void configure(QSharedPointer<Node>);
	void setPanic(bool panic);
	QSharedPointer<DiscoveryMandate> discoveryMandate();
	
	bool connecting() const;
	bool audioOn() const;
	bool cameraOn() const;
	bool bluetoothOn() const;
	bool NFCOn() const;
	bool networkOn() const;
	bool zooOn() const;
	bool panicOn() const;
	
private slots:
	void openSettings();
	void randomizeColor();
	void startCamera();
	void toggleAudio(const bool on);
	void toggleBluetooth(const bool on);
	void toggleNetwork(const bool on);
	void togglePanic(bool panic);
	void toggleZoo(bool panic);

signals:
	bool openConnectionSettings();
	void colorChanged(QColor color);
	bool panic(bool p);
};

#endif // CONNECTIONSTATUSWIDGET_HPP
