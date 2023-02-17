#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "node/NodeWindow.hpp"

#include "app/log/LogDestination.hpp"

#include "uptime/SharedPointerWrapper.hpp"



class Agent;
class QMenu;
class QAction;
class QWidget;


struct Page{
	QWidget *widget{nullptr};
	bool headerEnabled{false};
	bool backEnabled{false};
	bool menuEnabled{false};
	explicit Page(){
		
	}
	explicit Page(QWidget *widget, bool headerEnabled, bool backEnabled, bool menuEnabled)
		: widget(widget)
		, headerEnabled(headerEnabled)
		, backEnabled(backEnabled)
		, menuEnabled(menuEnabled)
	{
		
	}
};


namespace Ui
{
class AgentWindow;
}

/*!
 * \brief The AgentWindow class is the UI part of Agent. In it's current incarnation
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
	
	QMap<QString, Page> mPageMap;
	

public:
	explicit AgentWindow(QWidget *parent = nullptr);
	virtual ~AgentWindow() Q_DECL_OVERRIDE;

	// NodeWindow interface
public:
	void configure() Q_DECL_OVERRIDE;

public:
	QSharedPointer<Agent> agent();

	// LogDestination interface
public:
	void appendLog(const QString& text) Q_DECL_OVERRIDE;


	// Agent spesific private
private:
	QAction *addMenuItem(QString title, QString icon, QString toolTip, void (AgentWindow::*method)(), bool checkable = false);
	void prepareNavigation();
	void prepareMenu();
	
	void addPage(QWidget *widget, bool headerEnabled, bool backEnabled, bool menuEnabled);
	void setCurrentPage(QString name);
	

	
private slots:
	void onBackClicked();
	void onMenuClicked();
	void onStartQuitApplication();
	void onNotImplementedYet();
	void onStartCameraPairing();
	void onStartNormalPairing();
	void onConfigureHardware();
	void onPlanEditor();
	void onOnlineChanged();
	void onFaceVisibilityChanged();

};


#endif
// AGENTWINDOW_HPP
