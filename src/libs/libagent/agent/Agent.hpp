#ifndef AGENT_HPP
#define AGENT_HPP


#include "node/Node.hpp"

#include "uptime/SharedPointerWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QObject>


class AgentConfig;
class AgentConfigStore;
class AgentConfigService;
class AgentWindow;
class IAppLauncher;
class ControllerHandler;

class Agent : public Node//, public QEnableSharedFromThis<Agent>
{
	Q_OBJECT

private:
	// Helper to keep track of nodeConfigure() and nodeActivate() state
	ConfigureHelper mNodeConfigureHelper;

	// The window that represents this agent
	QSharedPointer<AgentWindow> mWindow;

	// The current state of panic mode. If enabled the agent will engage safety measures such as limping all actuators and blinking warning lights
	bool mPanic;

	// The configuration store for this agent. Contains all configuration data in secure asynchronous storage.
	QSharedPointer<AgentConfigStore> mAgentConfigStore;

	// The service to maintain agent config
	QSharedPointer<AgentConfigService> mAgentConfigService;
	
	
	// The currently active controller
	QSharedPointer<ControllerHandler> mControllerHandler;
	
public:
	explicit Agent();
	virtual ~Agent() override;

	// Node interface
public:
	// Called by Node::appConfigure();
	void nodeConfigure() override;
	
	// Called by Node::appActivate()
	void nodeActivate(const bool on) override;
	
	// Called by Node::appWindow()
	QSharedPointer<NodeWindow> nodeWindow() override;
	
	// Provide role
	NodeRole nodeRole() override;
	
	// Provide type
	NodeType nodeType() override;
	
	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() override;

public:
	// Accept triggering or un-triggering of panic state
	void setPanic(bool panic);
	// Get panic state
	bool panic();

public:
	// Provide the currently active configuration store for this agent
	QSharedPointer<AgentConfigStore> configurationStore();
	// Provide the currently active configuration for this agent
	QSharedPointer<AgentConfig> configuration();


public:
	// Provide the currently active actuator controller. Options include ardumy, servotor32 etc.
	QSharedPointer<ControllerHandler> controllerHandler();

};


#endif
// AGENT_HPP
