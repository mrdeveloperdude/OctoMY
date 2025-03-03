#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "node/NodeWindow.hpp"

#include "uptime/ConfigureHelper.hpp"

class AgentSelectActivity;
class ConnectionActivity;
class ControlBaptismActivity;
class ControlDeliveryActivity;
class ControlSipAndSeeActivity;
class ControlUnboxingWizard;
class IdentityActivity;
class MessageActivity;
class PairingActivity;
class PairingTrustActivity;
class Remote;
class RemoteController;
struct StackPage;
class TransitionActivity;



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
	QMenu *mMenu{nullptr};
	QAction *mQuitAction{nullptr};
	QAction *mPairingAction{nullptr};
	QAction *mIdentityAction{nullptr};
	QAction *mConnectionAction{nullptr};
	QAction *mToggleOnlineAction{nullptr};

	// Activities
	ControlBaptismActivity * mBaptismActivity{nullptr};
	ControlDeliveryActivity * mDeliveryActivity{nullptr};
	ControlSipAndSeeActivity * mSipAndSeeActivity{nullptr};
	ControlUnboxingWizard *mUnboxingWizard{nullptr};
	MessageActivity * mMessage{nullptr};
	PairingActivity *mPairingActivity{nullptr};
	IdentityActivity *mIdentityActivity{nullptr};
	PairingTrustActivity *mPairingTrustActivity{nullptr};
	ConnectionActivity *mConnectionActivity{nullptr};
	AgentSelectActivity *mAgentSelectActivity{nullptr};
	RemoteController *mControl{nullptr};
	TransitionActivity * mTransitionActivity{nullptr};
	
	bool mDebug{false};
	
	ConfigureHelper mConfigureHelper;

public:
	explicit RemoteWindow(QWidget *parent = nullptr);
	virtual ~RemoteWindow() override;
	
	// NodeWindow interface
public:
	void configure() override;

public:
	QSharedPointer<Remote> remote();

	// Remote spesific private
private:
	template <typename MethodType>
	QAction* addMenuItem(QString title, QString icon, QString toolTip, MethodType method, bool checkable = false);
	
	void prepareNavigation();
	void prepareMenu();
	void prepareActivities();

	// LogDestination interface
public:
	void appendLog(const QString& text);

public slots:
	void applicationShutdown();

public slots:
	QString popPage();
	bool pushPage(const QString &title, const QStringList arguments = QStringList());
	QString swapPage(const QString &title);


private slots:
	void navigateBack();
	void openMenu();
	void requestApplicationShutdown();
	void onPageChanged(const StackPage &page);
	void unboxingWizardDone();
	void pairing();
	void connection();
	void identity();
	void toggleOnline(bool online);

};

#endif
// REMOTEWINDOW_HPP
