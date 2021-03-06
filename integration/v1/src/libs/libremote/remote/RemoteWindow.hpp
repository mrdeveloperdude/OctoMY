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

	QAction *mPairingAction;
	QAction *mShowBirthCertificateAction;
	QAction *mUnbirthAction;
	QAction *mQuitAction;

	bool mWasEverUndelivered;

public:
	explicit RemoteWindow(QSharedPointer<Remote> mRemote, QWidget *parent = nullptr);
	virtual ~RemoteWindow() Q_DECL_OVERRIDE;

public: //IContextProvider interface

	Settings &settings() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer<Remote>);
	void updateWindowIcon();


private:

	// One time initialization of differnt parts
	void prepareControlLevelList();
	void prepareMenu();

	void updateClientWidgetStack();
	int  updateClientCombobox();
	void addClientToCombobox(QSharedPointer<Associate> peer);

	void updateControlLevel();
	void updateActiveClient();

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
	void onStartUnbirth();
	void onStartQuitApplication();


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
	void on_pushButtonUpdateStuff_clicked();
	void on_label_3_linkActivated(const QString &link);
};

#endif // REMOTEWINDOW_HPP
