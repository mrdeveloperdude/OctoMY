#ifndef HUBWINDOW_HPP
#define HUBWINDOW_HPP


#include "node/NodeWindow.hpp"

#include "log/LogType.hpp"
#include "uptime/ConfigureHelper.hpp"


class Hub;
class CommsSession;


class AgentSelectActivity;
class CameraActivity;
class PlanActivity;
class MapActivity;
class HUDActivity;
class ActuatorActivity;
class WidgetIllustrationActivity;
class SerialActivity;
class ComputeActivity;
class VoiceActivity;
class MarkdownActivity;
class LogActivity;
class NetworkUtilsActivity;
class NodesManagementActivity;
class InternalResourcesActivity;
class ConnectionActivity;
class ControlBaptismActivity;
class ControlDeliveryActivity;
class ControlSipAndSeeActivity;
class ControlUnboxingWizard;
class GaitActivity;
class HubMainMenu;
class DevelopmentMenuActivity;
class UtilitiesMenuActivity;
class IdentityActivity;
class IdentityMenuActivity;
class MenuActivity;
class MessageActivity;
class PKIManagerActivity;
class PairingActivity;
class PairingMenuActivity;
class PairingTrustActivity;
class QRUtilityActivity;
class TransitionActivity;
class TriggerManagerActivity;
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
	QSharedPointer<ControlBaptismActivity> mBaptismActivity;
	QSharedPointer<ControlDeliveryActivity> mDeliveryActivity;
	QSharedPointer<ControlSipAndSeeActivity> mSipAndSeeActivity;
	QSharedPointer<ControlUnboxingWizard> mUnboxingWizard;
	QSharedPointer<MessageActivity>  mMessage;
	QSharedPointer<PairingActivity> mPairingActivity;
	QSharedPointer<IdentityActivity> mIdentityActivity;
	QSharedPointer<PairingTrustActivity> mPairingTrustActivity;
	QSharedPointer<ConnectionActivity> mConnectionActivity;
	QSharedPointer<AgentSelectActivity> mAgentSelectActivity;
	QSharedPointer<PKIManagerActivity> mPKIManagerActivity;
	QSharedPointer<QRUtilityActivity> mQRUtilityActivity;
	QSharedPointer<TriggerManagerActivity> mTriggerManagerActivity;
	QSharedPointer<GaitActivity> mGaitActivity;
	QSharedPointer<CameraActivity> mCameraActivity;
	QSharedPointer<PlanActivity> mPlanActivity;
	QSharedPointer<MapActivity> mMapActivity;
	QSharedPointer<HUDActivity> mHUDActivity;
	QSharedPointer<ActuatorActivity> mActuatorActivity;
	QSharedPointer<WidgetIllustrationActivity> mWidgetIllustrationActivity;
	QSharedPointer<SerialActivity> mSerialActivity;
	QSharedPointer<ComputeActivity> mComputeActivity;
	QSharedPointer<VoiceActivity> mVoiceActivity;
	QSharedPointer<MarkdownActivity> mMarkdownActivity;
	QSharedPointer<LogActivity> mLogActivity;
	QSharedPointer<NetworkUtilsActivity> mNetworkUtilsActivity;
	QSharedPointer<NodesManagementActivity> mNodesManagementActivity;
	QSharedPointer<InternalResourcesActivity> mInternalResourcesActivity;
	
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
