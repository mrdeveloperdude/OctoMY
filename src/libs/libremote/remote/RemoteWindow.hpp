#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "widgets/TryToggle.hpp"
#include "node/IContextProvider.hpp"
#include "basic/Associate.hpp"
#include "node/NodeWindow.hpp"

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
class RemoteClientWidget;


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

class RemoteWindow : public NodeWindow, public IContextProvider
{
	Q_OBJECT

private:
	Ui::RemoteWindow *ui;
	QSharedPointer<Remote> mRemote;
	QMenu mMenu;

public:
	explicit RemoteWindow(QSharedPointer<Remote> mRemote, QWidget *parent = nullptr);
	virtual ~RemoteWindow();

public: //IContextProvider interface

	Settings &settings() Q_DECL_OVERRIDE;

public:

	void configure(QSharedPointer<Remote>);

	void updateWindowIcon();

public:
	//bool needConnection();

private:

	// One time initialization of differnt parts
	void prepareDelivery();
	void prepareDiscovery();
	void prepareControlLevelList();
	void prepareMenu();


	void updateClientWidgetList();
	void updateControlLevel();
	void updateActiveClient();
	int  updateClientsList();
	void addClientToList(QSharedPointer<Associate> peer);

	void setCurrentPage(QWidget *cur);
	void goToStartPage();

	void appendLog(const QString &str);


public:

	virtual void keyReleaseEvent(QKeyEvent *);
	virtual	void closeEvent(QCloseEvent *) Q_DECL_OVERRIDE;

	// Internal slots
private slots:
	void onStartPairing();
	void onStartPlanEditor();
	void onStartShowBirthCertificate();


	// Peer Store slots
public slots:
	void onAssociateAdded(QString id);
	void onAssociateRemoved(QString id);
	void onAssociateChanged();

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
	void on_comboBoxControlLevel_activated(const QString &arg1);
	void on_pushButtonConfirmQuit_clicked();
	void on_pushButtonMenu_clicked();
	void on_pushButtonBack_6_clicked();



	void on_pushButtonStartPairing_clicked();
};

#endif // REMOTEWINDOW_HPP
