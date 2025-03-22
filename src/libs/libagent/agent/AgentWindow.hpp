#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "agent/UnboxingStage.hpp"
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
	QMenu *mMenu{nullptr};
	QAction *mQuitAction{nullptr};
	QAction *mPairingAction{nullptr};
	QAction *mIdentityAction{nullptr};
	QAction *mHardwareAction{nullptr};
	QAction *mPlanEditorAction{nullptr};
	QAction *mToggleOnlineAction{nullptr};
	QAction *mShowFaceAction{nullptr};
	QAction *mConnectionAction{nullptr};
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
	QSharedPointer<IdentityMenuActivity> mIdentityMenuActivity;
	QSharedPointer<MessageActivity> mMessageActivity;
	QSharedPointer<PairingMenuActivity> mPairingMenuActivity;
	QSharedPointer<PairingActivity> mPairingActivity;
	QSharedPointer<PairingTrustActivity> mPairingTrustActivity;
	QSharedPointer<StanzaManagerActivity> mStanzaManagerActivity;
	QSharedPointer<SerialDeviceSelector> mSerialDeviceSelector;
	QSharedPointer<StanzaEditorActivity> mStanzaEditorActivity;
	QSharedPointer<ConnectionActivity> mConnectionActivity;
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
	template <typename MethodType>
	QAction* addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable = false);
	
	void prepareNavigation();
	void prepareMenu();
	void prepareActivities();

	
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
