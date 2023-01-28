#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include "pose/Pose.hpp"

#include "node/Node.hpp"
#include "comms/NetworkAddress.hpp"

#include "node/NodeLauncher.hpp"

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
	QSharedPointer<RemoteWindow> mWindow;

public:
	explicit Remote(NodeLauncher<Remote> &launcher, QObject *parent = nullptr);
	virtual ~Remote() Q_DECL_OVERRIDE;

public:
	virtual void init() Q_DECL_OVERRIDE;
	virtual void deInit() Q_DECL_OVERRIDE;

	//void startComms(const NetworkAddress &localAddress, const NetworkAddress &partnerAddress);

	virtual QSharedPointer<QWidget> showWindow() Q_DECL_OVERRIDE;

public:

	void identityChanged() Q_DECL_OVERRIDE;


	void setNodeCouriersRegistration(bool reg) Q_DECL_OVERRIDE;

	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;

};

#endif // REMOTE_HPP
