#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "agent/UnboxingStage.hpp"
#include "app/log/LogDestination.hpp"
#include "node/NodeWindow.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"


class Agent;
class AgentDeliveryWizard;
class AgentUnboxingWizard;
class FaceWidget;
class Message;
class PairingWizard;
class QAction;
class QMenu;
class QWidget;
struct StackPage;
class ControllerTypeSelector;
class SerialDeviceSelector;
class ControllerConfiguration;

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
	// Pages
	AgentDeliveryWizard * mDelivery{nullptr};
	AgentUnboxingWizard *mUnboxingWizard{nullptr};
	FaceWidget *mFaceWidget{nullptr};
	//HardwareWizard *mHardware{nullptr};
	ControllerTypeSelector * mControllerTypeSelector{nullptr};
	SerialDeviceSelector * mSerialDeviceSelector{nullptr};
	ControllerConfiguration * mControllerConfiguration{nullptr};
	Message * mQuitting{nullptr};
	PairingWizard *mPairing{nullptr};

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
	bool pushPage(const QString &title);
	QString swapPage(const QString &title);
	
private slots:
	void navigateBack();
	void openMenu();
	void onStartQuitApplication();
	void onQuitApplication(bool quitting);
	void onNotImplementedYet();
	void onStartCameraPairing();
	void onStartNormalPairing();
	void onConfigureHardware();
	void onPlanEditor();
	void onOnlineChanged();
	void onFaceVisibilityChanged();
	void onNextUnboxingStage(UnboxingStage stage);
	void unboxingWizardDone();
	
	void controllerTypeSelected(const QString &type);
	void serialControllerSelected(const QString &controller);
	
	void onPageChanged(const StackPage &page);


};


#endif
// AGENTWINDOW_HPP
