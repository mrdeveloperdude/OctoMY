#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "node/NodeWindow.hpp"

#include "log/LogType.hpp"
#include "uptime/ConfigureHelper.hpp"

class AgentSelectActivity;
class ConnectionActivity;
class ControlBaptismActivity;
class ControlDeliveryActivity;
class ControlSipAndSeeActivity;
class ControlUnboxingWizard;
class IdentityActivity;
class IdentityMenuActivity;
class MessageActivity;
class PairingActivity;
class PairingMenuActivity;
class PairingTrustActivity;
class Remote;
class RemoteController;
class RemoteMainMenu;
class TransitionActivity;
struct StackPage;



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

class RemoteWindow : public NodeWindow
{
	Q_OBJECT

private:
	Ui::RemoteWindow *ui;

	// Activities
	QSharedPointer<ControlBaptismActivity> mBaptismActivity;
	QSharedPointer<ControlDeliveryActivity> mDeliveryActivity;
	QSharedPointer<ControlSipAndSeeActivity> mSipAndSeeActivity;
	QSharedPointer<ControlUnboxingWizard> mUnboxingWizard;
	QSharedPointer<MessageActivity> mMessage;
	QSharedPointer<PairingActivity> mPairingActivity;
	QSharedPointer<IdentityActivity> mIdentityActivity;
	QSharedPointer<PairingTrustActivity> mPairingTrustActivity;
	QSharedPointer<ConnectionActivity> mConnectionActivity;
	QSharedPointer<AgentSelectActivity> mAgentSelectActivity;
	QSharedPointer<RemoteController> mControl;
	
	// Menus
	QSharedPointer<RemoteMainMenu> mMainMenu;
	QSharedPointer<IdentityMenuActivity> mIdentityMenuActivity;
	QSharedPointer<PairingMenuActivity> mPairingMenuActivity;
	QSharedPointer<TransitionActivity> mTransitionActivity;
	
	bool mDebug{false};
	
	ConfigureHelper mConfigureHelper;

public:
	explicit RemoteWindow(QWidget *parent = nullptr);
	virtual ~RemoteWindow() override;
	
	// NodeWindow interface
public:
	void configure() override;

public:
	QSharedPointer<Remote> remote()  const;

	// Remote spesific private
private:
	template <typename MethodType>
	QAction* addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable = false);
	
	void prepareNavigation();
	void prepareActivities();

	// LogDestination interface
public:
	void log(const QString &text, LogType type=LOG_TYPE_INFO) const;

public slots:
	void applicationShutdown();


public slots:
	void navigateBack();
	void menu();
	void requestApplicationShutdown();
	void unboxing();
	void onPageChanged(const StackPage &page);
	void unboxingWizardDone();
	void pairing();
	void connection();
	void identity();
	void toggleOnline(bool online);

};

#endif
// REMOTEWINDOW_HPP
