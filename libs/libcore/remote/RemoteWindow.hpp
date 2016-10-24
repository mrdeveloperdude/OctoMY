#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "widgets/TryToggle.hpp"
#include "../hub/IContextProvider.hpp"
#include "basic/NodeAssociate.hpp"

#include <QWidget>
#include <QSharedPointer>
#include <QGeoPositionInfo>
#include <QHostAddress>
#include <QMenu>


class Remote;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;
class CommsChannel;
class Client;
class ClientWidget;


namespace Ui
{
class RemoteWindow;
}

class RemoteWindow : public QWidget, public IContextProvider
{
	Q_OBJECT

private:
	Ui::RemoteWindow *ui;
	Remote *mRemote;
	QMenu mMenu;
	QMap<int, ClientWidget *> mClientWidgets;

public:
	explicit RemoteWindow(Remote *mRemote, QWidget *parent = nullptr);
	virtual ~RemoteWindow();

public: //IContextProvider interface

	Settings &settings() Q_DECL_OVERRIDE;

private:
	// Android spesifc notifications

	void notifyAndroid(QString);
	void toastAndroid(QString);


	void updateControlLevel();
	void updateActiveAgent();

	void updateIdentity();
	void addAgentToList(QSharedPointer<NodeAssociate> peer);

	// Menu stuff

	void prepareMenu();

	// Map stuff

	void prepareMap();
	void homeMap();



	void appendLog(const QString &str);


public:

	virtual void keyReleaseEvent(QKeyEvent *);


	// Internal slots
private slots:
	void onStartPairing();
	void onStartPlanEditor();
	void onStartShowBirthCertificate();

	// Peer Store slots
public slots:
	void onPeerAdded(QString id);
	void onPeerRemoved(QString id);
	void onPeersChanged();
	void onStoreReady(bool);


	// CommsChannel slots
public slots:
	void onError(QString);
	void onClientAdded(Client *);
	void onConnectionStatusChanged(bool);

	// Internal Sensor slots
private slots:
	void onPositionUpdated(const QGeoPositionInfo &info);
	void onCompassUpdated(QCompassReading *);
	void onAccelerometerUpdated(QAccelerometerReading *);
	void onGyroscopeUpdated(QGyroscopeReading *r);

	void onServoPositionChanged(int);

	//Internal UI slots
private slots:
	void on_comboBoxAgent_currentIndexChanged(int index);
	void on_comboBoxControlLevel_activated(const QString &arg1);
	void on_pushButtonConfirmQuit_clicked();
	void on_pushButtonMenu_clicked();
	void on_pushButtonBack_6_clicked();



};

#endif // REMOTEWINDOW_HPP
