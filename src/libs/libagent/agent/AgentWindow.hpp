#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "agent/UnboxingStage.hpp"
#include "node/NodeWindow.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"


class Agent;
class AgentBaptismActivity;
class AgentDeliveryActivity;
class AgentMainMenu;
class AgentSipAndSeeActivity;
class AgentUnboxingWizard;
class CameraPairingActivity;
class ConnectionActivity;
class ControllerActivity;
class ControllerTypeSelector;
class FaceActivity;
class HardwareActivity;
class IdentityActivity;
class IdentityMenuActivity;
class LobeTypeSelector;
class MessageActivity;
class NetworkPairingActivity;
class PairingActivity;
class PairingMenuActivity;
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
class AgentWindow : public NodeWindow
{
	Q_OBJECT

private:
	Ui::AgentWindow *ui;
	// Activities
	QSharedPointer<AgentBaptismActivity> mBaptismActivity;
	QSharedPointer<AgentDeliveryActivity> mDeliveryActivity;
	QSharedPointer<AgentSipAndSeeActivity> mSipAndSeeActivity;
	QSharedPointer<AgentUnboxingWizard> mUnboxingWizard;
	QSharedPointer<LobeTypeSelector> mLobeTypeSelectorActivity;
	QSharedPointer<CameraPairingActivity> mCameraPairingActivity;
	QSharedPointer<NetworkPairingActivity> mNetworkPairingActivity;
	QSharedPointer<ControllerActivity> mControllerConfiguration;
	QSharedPointer<HardwareActivity> mHardwareConfiguration;
	QSharedPointer<IdentityActivity> mIdentityActivity;
	QSharedPointer<ControllerTypeSelector> mControllerTypeSelector;
	QSharedPointer<FaceActivity> mFaceActivity;
	QSharedPointer<MessageActivity> mMessageActivity;
	QSharedPointer<PairingActivity> mPairingActivity;
	QSharedPointer<PairingTrustActivity> mPairingTrustActivity;
	QSharedPointer<StanzaManagerActivity> mStanzaManagerActivity;
	QSharedPointer<SerialDeviceSelector> mSerialDeviceSelector;
	QSharedPointer<StanzaEditorActivity> mStanzaEditorActivity;
	QSharedPointer<ConnectionActivity> mConnectionActivity;
	
	// Menus
	QSharedPointer<AgentMainMenu> mMainMenu;
	QSharedPointer<IdentityMenuActivity> mIdentityMenuActivity;
	QSharedPointer<PairingMenuActivity> mPairingMenuActivity;
	QSharedPointer<TransitionActivity> mTransitionActivity;

	
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

public:
	void appendLog(const QString& text);
	

	// Agent spesific private
private:
	void prepareNavigation();
	void prepareActivities();

public slots:
	void applicationShutdown();

public slots:
	void navigateBack();
	void menu();
	void requestApplicationShutdown();
	void unboxing();
	void notImplementedAction();
	void pairing();
	void identity();
	void connection();
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
