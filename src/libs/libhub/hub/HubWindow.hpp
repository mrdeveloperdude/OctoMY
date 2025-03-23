#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP


#include "node/NodeWindow.hpp"

#include "log/LogType.hpp"
#include "uptime/ConfigureHelper.hpp"


class Hub;
class CommsSession;


class ActuatorActivity;
class AgentSelectActivity;
class CameraActivity;
class ConstructActivity;
class ComputeActivity;
class ConnectionActivity;
class ControlBaptismActivity;
class ControlDeliveryActivity;
class ControlSipAndSeeActivity;
class ControlUnboxingWizard;
class DevelopmentMenuActivity;
class GaitActivity;
class HUDActivity;
class HubMainMenu;
class IdentityActivity;
class IdentityMenuActivity;
class InternalResourcesActivity;
class LogActivity;
class MapActivity;
class MarkdownActivity;
class MenuActivity;
class MessageActivity;
class NetworkUtilsActivity;
class NodesManagementActivity;
class PKIManagerActivity;
class PairingActivity;
class PairingMenuActivity;
class PairingTrustActivity;
class PlanActivity;
class QRUtilityActivity;
class SerialActivity;
class SplitterActivity;
class TransitionActivity;
class TriggerManagerActivity;
class UtilitiesMenuActivity;
class VoiceActivity;
class WidgetIllustrationActivity;
struct StackPage;

namespace qmapcontrol
{
class MapControl;
}

namespace Ui
{
class HubWindow;
}

/*!
 * \brief The HubWindow class is the UI part of Hub. In it's current incarnation
 * it is required, but the long term goal is for remote to be able to run in
 * head-less mode, and in that mode the UI part will not be used.
 *
 * This is useful for when your hub is acting as a service.
 */
class HubWindow: public NodeWindow
{
	Q_OBJECT
private:
	Ui::HubWindow *ui;
	
	QMenu *mMenu{nullptr};
	QAction *mQuitAction{nullptr};
	QAction *mPairingAction{nullptr};
	QAction *mIdentityAction{nullptr};
	QAction *mConnectionAction{nullptr};
	QAction *mMenuAction{nullptr};
	QAction *mToggleOnlineAction{nullptr};
	
	// Activities
	QSharedPointer<ActuatorActivity> mActuatorActivity;
	QSharedPointer<AgentSelectActivity> mAgentSelectActivity;
	QSharedPointer<CameraActivity> mCameraActivity;
	QSharedPointer<ConstructActivity> mConstructActivity;
	QSharedPointer<ComputeActivity> mComputeActivity;
	QSharedPointer<ConnectionActivity> mConnectionActivity;
	QSharedPointer<ControlBaptismActivity> mBaptismActivity;
	QSharedPointer<ControlDeliveryActivity> mDeliveryActivity;
	QSharedPointer<ControlSipAndSeeActivity> mSipAndSeeActivity;
	QSharedPointer<ControlUnboxingWizard> mUnboxingWizard;
	QSharedPointer<GaitActivity> mGaitActivity;
	QSharedPointer<HUDActivity> mHUDActivity;
	QSharedPointer<IdentityActivity> mIdentityActivity;
	QSharedPointer<InternalResourcesActivity> mInternalResourcesActivity;
	QSharedPointer<LogActivity> mLogActivity;
	QSharedPointer<MapActivity> mMapActivity;
	QSharedPointer<MarkdownActivity> mMarkdownActivity;
	QSharedPointer<MessageActivity>  mMessage;
	QSharedPointer<NetworkUtilsActivity> mNetworkUtilsActivity;
	QSharedPointer<NodesManagementActivity> mNodesManagementActivity;
	QSharedPointer<PKIManagerActivity> mPKIManagerActivity;
	QSharedPointer<PairingActivity> mPairingActivity;
	QSharedPointer<PairingTrustActivity> mPairingTrustActivity;
	QSharedPointer<PlanActivity> mPlanActivity;
	QSharedPointer<QRUtilityActivity> mQRUtilityActivity;
	QSharedPointer<SerialActivity> mSerialActivity;
	QSharedPointer<SplitterActivity> mSplitterActivity;
	QSharedPointer<TriggerManagerActivity> mTriggerManagerActivity;
	QSharedPointer<VoiceActivity> mVoiceActivity;
	QSharedPointer<WidgetIllustrationActivity> mWidgetIllustrationActivity;
	
	// Menus
	QSharedPointer<HubMainMenu> mMainMenu;
	QSharedPointer<DevelopmentMenuActivity> mDevelopmentMenu;
	QSharedPointer<UtilitiesMenuActivity> mUtilitiesMenu;
	QSharedPointer<IdentityMenuActivity> mIdentityMenuActivity;
	QSharedPointer<PairingMenuActivity> mPairingMenuActivity;
	
	QSharedPointer<TransitionActivity> mTransitionActivity;
	
	bool mDebug{false};
	
	ConfigureHelper mConfigureHelper;
	
	
	quint64 mLastTime;
	qreal mAngle=0.0;

public:
	explicit HubWindow(QWidget *parent = nullptr);
	virtual ~HubWindow();

	// NodeWindow
public:
	void configure() override;

public:
	QSharedPointer<Hub> hub() const;
	
	// Hub spesific private
private:
	template <typename MethodType>
	QAction* addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable = false);
	
	void prepareActivities();
	void prepareNavigation();
	void prepareNavigationMenu();

public:
	void log(const QString &text, LogType type=LOG_TYPE_INFO) const;
	
public slots:
	void applicationShutdown();
	
public slots:
	QString popPage();
	bool pushPage(const QString &title, const QStringList arguments = QStringList());
	QString swapPage(const QString &title);

public slots:
	void navigateBack();
	void openNavigation();
	void requestApplicationShutdown();
	void onPageChanged(const StackPage &page);
	void unboxingWizardDone();
	void unboxing();
	void pairing();
	void connection();
	void menu();
	void identity();
	void toggleOnline(bool online);
};

#endif
// HUBWINDOW_HPP
