#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "agent/UnboxingStage.hpp"
#include "app/log/LogDestination.hpp"
#include "node/NodeWindow.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"


class Agent;
class AgentSipAndSeeActivity;
class AgentBaptismActivity;
class AgentDeliveryActivity;
class AgentDeliveryActivity;
class AgentUnboxingWizard;
class CameraPairingActivity;
class ControllerActivity;
class ControllerTypeSelector;
class FaceWidget;
class MessageActivity;
class PairingActivity;
class PairingTrustActivity;
class QAction;
class QMenu;
class QWidget;
class SerialDeviceSelector;
class TransitionActivity;
struct StackPage;

namespace Ui
{
class AgentWindow;
}

/*!
 * \brief The AgentWindow class is the UI part of Agent. In its current incarnation
 * it is required, but the long term goal is for agent to be able to run in
 * head-less mode, and in that mode the UI part will not be used.
 *
 * This is useful for when your robots has no screen and/or is running on
 * resource constrained hardware.
 */
class AgentWindow : public NodeWindow, public LogDestination
{
	Q_OBJECT

private:
	Ui::AgentWindow *ui;
	QMenu *mMenu;
	QAction *mQuitAction;
	QAction *mStartCameraPairingAction;
	QAction *mStartNormalPairingAction;
	QAction *mHardwareAction;
	QAction *mPlanEditorAction;
	QAction *mToggleOnlineAction;
	QAction *mShowFaceAction;
	// Activities
	AgentSipAndSeeActivity * mSipAndSeeActivity{nullptr};
	AgentBaptismActivity * mBaptismActivity{nullptr};
	AgentDeliveryActivity * mDeliveryActivity{nullptr};
	AgentUnboxingWizard *mUnboxingWizard{nullptr};
	CameraPairingActivity *mCameraPairingActivity{nullptr};
	ControllerActivity * mControllerConfiguration{nullptr};
	ControllerTypeSelector * mControllerTypeSelector{nullptr};
	FaceWidget *mFaceWidget{nullptr};
	MessageActivity * mMessageActivity{nullptr};
	TransitionActivity * mTransitionActivity{nullptr};
	PairingActivity *mPairingActivity{nullptr};
	PairingTrustActivity *mPairingTrustActivity{nullptr};
	SerialDeviceSelector * mSerialDeviceSelector{nullptr};
	const static QString mQuitApplicationText;

	bool mDebug{true};

	ConfigureHelper mConfigureHelper;

public:
	explicit AgentWindow(QWidget *parent = nullptr);
	virtual ~AgentWindow() override;

	// NodeWindow interface
public:
	void configure() override;

public:
	QSharedPointer<Agent> agent();

	// LogDestination interface
public:
	void appendLog(const QString& text) override;


	// Agent spesific private
private:
	QAction *addMenuItem(QString title, QString icon, QString toolTip, void (AgentWindow::*method)(), bool checkable = false);
	void prepareNavigation();
	void prepareMenu();
	void prepareActivities();
	void updateFaceVisibility();
	
public slots:
	QString popPage();
	bool pushPage(const QString &title, const QStringList arguments = QStringList());
	QString swapPage(const QString &title);
	
public slots:
	void quitApplication();

private slots:
	void navigateBack();
	void openMenu();
	void onStartQuitApplication();
	void onNotImplementedYet();
	void onStartCameraPairing();
	void onStartNormalPairing();
	void onConfigureHardware();
	void onPlanEditor();
	void onOnlineChanged();
	void onFaceVisibilityChanged();
	void unboxingWizardDone();

public:
	void controllerTypeSelected(const QString &type);
	void serialControllerSelected(const QString &controller);
	
	void onPageChanged(const StackPage &page);

};


#endif
// AGENTWINDOW_HPP
