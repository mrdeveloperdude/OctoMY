#ifndef AGENTWINDOW_HPP
#define AGENTWINDOW_HPP

#include "node/NodeWindow.hpp"

#include "app/log/LogDestination.hpp"

#include "uptime/SharedPointerWrapper.hpp"

class Agent;

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

};


#endif
// AGENTWINDOW_HPP
