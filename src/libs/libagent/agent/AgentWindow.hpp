#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "agent/UnboxingStage.hpp"
#include "app/log/LogDestination.hpp"
#include "node/NodeWindow.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"


class Agent;
class AgentBaptismActivity;
class AgentDeliveryActivity;
class AgentDeliveryActivity;
class AgentSipAndSeeActivity;
class AgentUnboxingWizard;
class CameraPairingActivity;
class ControllerActivity;
class ControllerTypeSelector;
class FaceActivity;
class HardwareActivity;
class IdentityActivity;
class LobeTypeSelector;
class MessageActivity;
class NetworkPairingActivity;
class PairingActivity;
class PairingTrustActivity;
class QAction;
class QMenu;
class QWidget;
class SerialDeviceSelector;
class StanzaEditorActivity;
class StanzaManagerActivity;
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
	QMenu *mMenu{nullptr};
	QAction *mQuitAction{nullptr};
	QAction *mPairingAction{nullptr};
	QAction *mIdentityAction{nullptr};
	QAction *mHardwareAction{nullptr};
	QAction *mPlanEditorAction{nullptr};
	QAction *mToggleOnlineAction{nullptr};
	QAction *mShowFaceAction{nullptr};
	// Activities
	AgentBaptismActivity * mBaptismActivity{nullptr};
	AgentDeliveryActivity * mDeliveryActivity{nullptr};
	AgentSipAndSeeActivity * mSipAndSeeActivity{nullptr};
	AgentUnboxingWizard *mUnboxingWizard{nullptr};
	LobeTypeSelector *mLobeTypeSelectorActivity{nullptr};
	CameraPairingActivity *mCameraPairingActivity{nullptr};
	NetworkPairingActivity *mNetworkPairingActivity{nullptr};
	ControllerActivity * mControllerConfiguration{nullptr};
	HardwareActivity * mHardwareConfiguration{nullptr};
	ControllerTypeSelector * mControllerTypeSelector{nullptr};
	FaceActivity *mFaceActivity{nullptr};
	IdentityActivity *mIdentityActivity{nullptr};
	MessageActivity * mMessageActivity{nullptr};
	PairingActivity *mPairingActivity{nullptr};
	PairingTrustActivity *mPairingTrustActivity{nullptr};
	StanzaManagerActivity * mStanzaManagerActivity{nullptr};
	SerialDeviceSelector * mSerialDeviceSelector{nullptr};
	StanzaEditorActivity * mStanzaEditorActivity{nullptr};
	TransitionActivity * mTransitionActivity{nullptr};
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
	template <typename MethodType>
	QAction* addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable = false);
	
	void prepareNavigation();
	void prepareMenu();
	void prepareActivities();
	void updateFaceVisibility();
	
public slots:
	QString popPage();
	bool pushPage(const QString &title, const QStringList arguments = QStringList());
	QString swapPage(const QString &title);

public slots:
	void applicationShutdown();

private slots:
	void navigateBack();
	void openMenu();
	void requestApplicationShutdown();
	void notImplementedAction();
	void pairing();
	void identity();
	void configureHardware();
	void editPlan();
	
	void toggleOnline(bool online);
	void face();
	void unboxingWizardDone();

public:
	void controllerTypeSelected(const QString &type);
	void serialControllerSelected(const QString &controller);
	
	void onPageChanged(const StackPage &page);

};


#endif
// AGENTWINDOW_HPP
