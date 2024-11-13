#ifndef REMOTEWINDOW_HPP
#define REMOTEWINDOW_HPP

#include "app/log/LogDestination.hpp"
#include "node/NodeWindow.hpp"

#include "remote/ControlUnboxingStage.hpp"
#include "uptime/ConfigureHelper.hpp"

class AgentSelectActivity;
class ControlBaptismActivity;
class ControlDeliveryActivity;
class ControlSipAndSeeActivity;
class ControlUnboxingWizard;
class MessageActivity;
class PairingActivity;
class PairingTrustActivity;
class Remote;
class RemoteController;
class StackPage;



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

class RemoteWindow : public NodeWindow, public LogDestination
{
	Q_OBJECT

private:
	Ui::RemoteWindow *ui;
	// Activities
	ControlBaptismActivity * mBaptismActivity{nullptr};
	ControlDeliveryActivity * mDeliveryActivity{nullptr};
	ControlSipAndSeeActivity * mSipAndSeeActivity{nullptr};
	ControlUnboxingWizard *mUnboxingWizard{nullptr};
	MessageActivity * mMessage{nullptr};
	PairingActivity *mPairing{nullptr};
	PairingTrustActivity *mPairingTrustActivity{nullptr};
	AgentSelectActivity *mAgentSelectActivity{nullptr};
	RemoteController *mControl{nullptr};
	
	bool mDebug{false};
	
	ConfigureHelper mConfigureHelper;

public:
	explicit RemoteWindow(QWidget *parent = nullptr);
	virtual ~RemoteWindow() override;

private:
	void prepareActivities();

public:
	void configure() override;
	QSharedPointer<Remote> remote();
	
public slots:
	void quitApplication();
	

	// LogDestination interface
public:
	void appendLog(const QString& text) override;

public slots:
	QString popPage();
	bool pushPage(const QString &title, const QStringList arguments = QStringList());
	QString swapPage(const QString &title);


private slots:
	void onStartQuitApplication();
	void onPageChanged(const StackPage &page);
	void unboxingWizardDone();
};

#endif
// REMOTEWINDOW_HPP
