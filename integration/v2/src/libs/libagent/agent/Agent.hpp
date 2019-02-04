#ifndef AGENT_HPP
#define AGENT_HPP


#include "node/Node.hpp"
#include "app/launcher/AppLauncher.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QObject>


class AgentWindow;
class AgentConfigStore;
class IActuatorController;
class IAppLauncher;

class Agent : public Node, public QEnableSharedFromThis<Agent>
{
	Q_OBJECT

private:
	// The window that represents this agent
	QSharedPointer<AgentWindow> mWindow;

	// The current state of panic mode. If enabled the agent will engage safety meqasures such as limping all actuators and blinking warning lights
	bool mPanic;

	// The configuration store for this agent. Contains all configuration data in secure asynchronous storage.
	QSharedPointer<AgentConfigStore> mAgentConfigStore;

	// The currently active actuator controller
	QSharedPointer<IActuatorController> mActuatorController;

public:
	explicit Agent(QObject *parent = nullptr);
	virtual ~Agent() Q_DECL_OVERRIDE;

	// Node interface
public:
	void nodeConfigure() Q_DECL_OVERRIDE;
	void nodeInit() Q_DECL_OVERRIDE;
	void nodeDeInit() Q_DECL_OVERRIDE;
	QSharedPointer<QWidget> nodeWindow() Q_DECL_OVERRIDE;
	NodeRole nodeRole() Q_DECL_OVERRIDE;
	NodeType nodeType() Q_DECL_OVERRIDE;

	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;

public:
	// Accept triggering or un-triggering of panic state
	void setPanic(bool panic);
	// Get panic state
	bool panic();


public:
	// Provide the currently active configuration store for this agent
	QSharedPointer<AgentConfigStore> configurationStore();


public:
	// Provide the currently active actuator controller. Options include ardumy, servotor32 etc.
	QSharedPointer<IActuatorController> actuatorController();
	// Unload the currently active actuator controller
	void unloadController();
	// Unload and reload the currently active actuator controller from the stored configuration
	void reloadController();

};


#endif
// AGENT_HPP
