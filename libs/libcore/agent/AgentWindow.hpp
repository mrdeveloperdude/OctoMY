#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "widgets/TryToggle.hpp"
#include "basic/LogDestination.hpp"


#include <QWidget>
#include <QGeoPositionInfo>
#include <QMenu>

class Agent;
class QAccelerometerReading;
class QCompassReading;
class QGyroscopeReading;
class CommsChannel;
class HexyTool;
class QAction;


namespace Ui
{
class AgentWindow;
}

class AgentWindow : public QWidget, public LogDestination
{
	Q_OBJECT

private:
	Ui::AgentWindow *ui;
	Agent *mAgent;
	HexyTool *mHexy;
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

public:
	explicit AgentWindow(Agent *mAgent, QWidget *parent = 0);
	virtual ~AgentWindow();

	void appendLog(const QString& text);

	void updateVisibility();

private:
	void updateIdentity();
	void prepareMenu();
private:
	void notifyAndroid(QString);
	void toastAndroid(QString);

public:

	virtual void keyReleaseEvent(QKeyEvent *);

private slots:

	void onStartCameraPairing();
	void onStartPairing();
	void onStartHardware();
	void onStartPlanEditor();
	void onStartShowBirthCertificate();

private slots:

	void onOnlineChanged(bool);
	void onOnlineButtonVisibilityChanged(bool);
	void onFaceVisibilityChanged(bool);
	void onLogVisibilityChanged(bool);
	void onStatsVisibilityChanged(bool);


	void onConnectionStateChanged(TryToggleState, TryToggleState);

	void onColorChanged(QColor);

private slots:
	void on_pushButtonConfirmQuit_clicked();
	void on_pushButtonBack_clicked();
	void on_pushButtonMenu_clicked();
	void on_pushButtonBack_2_clicked();
	void on_pushButtonBack_4_clicked();
	void on_pushButtonBack_3_clicked();
	void on_pushButtonBack_5_clicked();
	void on_pushButtonBack_6_clicked();
};


#endif // AGENTWINDOW_HPP
