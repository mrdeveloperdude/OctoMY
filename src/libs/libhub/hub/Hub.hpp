#ifndef HUB_HPP
#define HUB_HPP

#include "node/Node.hpp"

#include "app/launcher/AppLauncher.hpp"


/*
  A hub is categorized as a control together with remote, and use the same
  interface to control agents.

  But what separates a hub from a remote is that the hub represents a stationary
  automated program that may concert the operation of several agents. In
  contrast remote is used to control a single agent at the time.

*/


class CommsSession;
class HubWindow;

class Hub: public Node //, public QEnableSharedFromThis<Hub>
{
	Q_OBJECT
private:
	// Helper to keep track of nodeConfigure() and nodeActivate() state
	ConfigureHelper mNodeConfigureHelper;

	QSharedPointer<HubWindow> mWindow;

public:
	explicit Hub();
	virtual ~Hub() Q_DECL_OVERRIDE;

	// Node interface
public:
	void nodeConfigure() Q_DECL_OVERRIDE;
	void nodeActivate(const bool on) Q_DECL_OVERRIDE;

	QSharedPointer<NodeWindow> nodeWindow() Q_DECL_OVERRIDE;
	NodeRole nodeRole() Q_DECL_OVERRIDE;
	NodeType nodeType() Q_DECL_OVERRIDE;

	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;

};

#endif
// HUB_HPP
