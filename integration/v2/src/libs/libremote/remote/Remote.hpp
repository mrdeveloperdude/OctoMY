#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "hardware/sensors/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include "pose/Pose.hpp"

#include "node/Node.hpp"
#include "comms/NetworkAddress.hpp"

#include "app/launcher/AppLauncher.hpp"

#include <QObject>
#include <QVector2D>
#include <QColor>

class DirectPoseCourier;
class RemoteWindow;


/*

  A remote is used to control agents, and it uses the same api as hub.

  But what separates a remote from a hub is that the remote represents a
  compact portable handheld device for controlling agents instead of the more
  stationary, more automated approach used by hub.

*/

class Remote : public Node, public QEnableSharedFromThis<Remote>
{
	Q_OBJECT
private:

	// Helper to keep track of nodeConfigure() and nodeActivate() state
	ConfigureHelper mNodeConfigureHelper;

	// The window that represents this remote
	QSharedPointer<RemoteWindow> mWindow;

public:
	explicit Remote();
	virtual ~Remote() Q_DECL_OVERRIDE;


	// Node interface
public:
	void nodeConfigure() Q_DECL_OVERRIDE;
	void nodeActivate(const bool on) Q_DECL_OVERRIDE;

	QSharedPointer<QWidget> nodeWindow() Q_DECL_OVERRIDE;
	NodeRole nodeRole() Q_DECL_OVERRIDE;
	NodeType nodeType() Q_DECL_OVERRIDE;

	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;


	void setNodeCouriersRegistration(bool reg) Q_DECL_OVERRIDE;

};

#endif
// REMOTE_HPP
