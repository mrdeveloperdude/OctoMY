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
class CommsSession;
class ClientWidget;


namespace Ui
{
class RemoteWindow;
}

/*!
 * \brief The RemoteWindow class is the UI part of Remote. In it's current incarnation
 * it is required, but the long term goal is for remote to be able to run in
 * head-less mode, and in that mode the UI part will not be used.
 *
 * This is useful for when your remote is acting as a script host or in other
 * similar situations where the UI may not be needed.
 */

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

	int updateAgentsList();
	void addAgentToList(QSharedPointer<NodeAssociate> peer);

	void hookSensorSignals();

	// One time initialization of differnt parts

	void prepareDelivery();
	void prepareDiscovery();
	void preparePairing();
	void prepareAgentList();
	void prepareControlLevelList();
	void prepareMenu();

	void goToStartPage();

	void appendLog(const QString &str);


public:

	virtual void keyReleaseEvent(QKeyEvent *);


	// Internal slots
private slots:
	void onStartPairing();
	void onStartPlanEditor();
	void onStartShowBirthCertificate();

	// Delivery wizard slots

public slots:
	void onDeliveryDone(bool pairNow);

	// Keystore slots
public slots:
	void onKeystoreReady(bool);

	// Peer Store slots
public slots:
	void onPeerAdded(QString id);
	void onPeerRemoved(QString id);
	void onPeersChanged();
	void onPeerStoreReady(bool);

	// CommsChannel slots
public slots:
	void onError(QString);
	void onClientAdded(CommsSession *);
	void onConnectionStatusChanged(bool);

	// Internal Sensor slots
private slots:
	void onPositionUpdated(const QGeoPositionInfo &info);
	void onCompassUpdated(QCompassReading *);
	void onAccelerometerUpdated(QAccelerometerReading *);
	void onGyroscopeUpdated(QGyroscopeReading *r);

	//Internal UI slots
private slots:
	void on_comboBoxAgent_currentIndexChanged(int index);
	void on_comboBoxControlLevel_activated(const QString &arg1);
	void on_pushButtonConfirmQuit_clicked();
	void on_pushButtonMenu_clicked();
	void on_pushButtonBack_6_clicked();



	void on_pushButtonStartPairing_clicked();
};

#endif // REMOTEWINDOW_HPP
