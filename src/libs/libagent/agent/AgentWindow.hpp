#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "widgets/TryToggleState.hpp"

#include "basic/LogDestination.hpp"

#include "basic/Associate.hpp"

#include "pose/PoseMapping.hpp"

#include <QWidget>
#include <QGeoPositionInfo>
#include <QMenu>
#include <QSet>

class Courier;
class Agent;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;
class CommsChannel;
class Servotor32ControllerWidget;
class QAction;
class ISyncParameter;
class CommsSession;

namespace Ui
{
class AgentWindow;
}

/*!
 * \brief The AgentWindow class is the UI part of Agent. In it's current incarnation
 * it is required, but the long term goal is for agent to be able to run in
 * head-less mode, and in that mode the UI part will not be used.
 *
 * This is useful for when your robots has no screen and/or is low on resources.
 */
class AgentWindow : public QWidget, public LogDestination
{
	Q_OBJECT

private:
	Ui::AgentWindow *ui;
	QSharedPointer<Agent> mAgent;
	Servotor32ControllerWidget *mHexy;
	QMenu mMenu;

	QAction *mCameraAction;
	QAction *mPairingAction;
	QAction *mHardwareAction;
	QAction *mPlanAction;
	QAction *mOnlineAction;
	QAction *mShowOnlineButtonAction;
	QAction *mShowFaceAction;
	QAction *mShowLogAction;
	QAction *mShowStatsAction;
	QAction *mShowBirthCertificateAction;
	QAction *mUnbirthAction;

	bool mWasEverUndelivered;

public:
	explicit AgentWindow(QSharedPointer<Agent> agent, QWidget *parent = 0);
	virtual ~AgentWindow();


public:

	void configure(QSharedPointer<Agent> agent);
public:
	void appendLog(const QString& text);
	void updateFaceVisibility();

private:

	void setCurrentPage(QWidget *cur);
	void gotoNextConfigPage();

	void updateIcon();
	void prepareMenu();

	void updateOnlineStatus();

	QSharedPointer<PoseMapping> poseMapping();

private:
	void notifyAndroid(QString);
	void toastAndroid(QString);

public:

	virtual void keyReleaseEvent(QKeyEvent *) Q_DECL_OVERRIDE;
	virtual	void closeEvent(QCloseEvent *) Q_DECL_OVERRIDE;

	// Actions
private slots:

	void onStartDelivery();
	void onStartCameraPairing();
	void onStartPairing();
	void onStartHardware();
	void onStartPlanEditor();
	void onStartShowBirthCertificate();

	// Actions
private slots:

	void onOnlineChanged(bool);
	void onOnlineButtonVisibilityChanged(bool);
	void onFaceVisibilityChanged(bool);
	void onLogVisibilityChanged(bool);
	void onStatsVisibilityChanged(bool);

// Face Widget
public slots:
	void onConnectionStateChanged(TryToggleState, TryToggleState);
	void onColorChanged(QColor);
	void onPanic();

	//Agent State Courier slots
public slots:
	void onSyncParameterChanged(ISyncParameter *sp);


	// CommsChannel slots
private slots:
	void onCommsError(QString);
	void onCommsClientAdded(CommsSession *);
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);

	// UI slots
private slots:
	void on_pushButtonConfirmQuit_clicked();
	void on_pushButtonBack_clicked();
	void on_pushButtonMenu_clicked();
	void on_pushButtonBack_2_clicked();
	void on_pushButtonBack_4_clicked();
	void on_pushButtonBack_3_clicked();
	void on_pushButtonBack_5_clicked();
	void on_pushButtonBack_6_clicked();
	void on_pushButtonSkipConfig_clicked();
	void on_pushButtonPairWithControls_clicked();
	void on_pushButtonConfigureHardware_clicked();
	void on_pushButtonDeliver_clicked();
};


#endif // AGENTWINDOW_HPP
