#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include "puppet/Pose.hpp"

#include "basic/Node.hpp"
#include "basic/NetworkAddress.hpp"

#include "basic/NodeLauncher.hpp"

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

class Remote : public Node
{
	Q_OBJECT
private:
	DirectPoseCourier *poseCourier;
	RemoteWindow *window;

public:
	explicit Remote(NodeLauncher<Remote> &launcher, QObject *parent = nullptr);
	virtual ~Remote();

	void start(const NetworkAddress &localAddress, const NetworkAddress &partnerAddress);

	virtual QWidget *showWindow();

	//External commands
public slots:

	void onDirectPoseChanged(Pose);


};

#endif // REMOTE_HPP
