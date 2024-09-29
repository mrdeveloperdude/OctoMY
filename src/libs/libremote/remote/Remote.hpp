#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "node/Node.hpp"

#include <QObject>
#include <QVector2D>
#include <QColor>

class RemoteWindow;
class NodeWindow;


/*

  A remote is used to control agents, and it uses the same api as hub.

  But what separates a remote from a hub is that the remote represents a
  compact portable handheld device for controlling agents instead of the more
  stationary, more automated approach used by hub.

*/

class Remote : public Node//, public QEnableSharedFromThis<Remote>
{
	Q_OBJECT
private:
	// Helper to keep track of nodeConfigure() and nodeActivate() state
	ConfigureHelper mNodeConfigureHelper;

	// The window that represents this remote
	QSharedPointer<RemoteWindow> mWindow;

public:
	explicit Remote();
	virtual ~Remote() override;

	// Node interface
public:
	void nodeConfigure() override;
	void nodeActivate(const bool on) override;
	QSharedPointer<NodeWindow> nodeWindow() override;
	NodeRole nodeRole() override;
	NodeType nodeType() override;

	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() override;
	void registerNodeCouriers(bool reg) override;

};

#endif
// REMOTE_HPP
