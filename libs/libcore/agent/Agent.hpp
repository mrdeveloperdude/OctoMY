#ifndef AGENT_HPP
#define AGENT_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"
#include "widgets/TryToggle.hpp"
#include "camera/CameraList.hpp"

#include "basic/Node.hpp"
#include "basic/NodeLauncher.hpp"


#include <QObject>
#include <QCommandLineParser>

class ZooClient;
class DiscoveryClient;
class AgentWindow;

class Agent : public Node
{
	Q_OBJECT
public:
	AgentWindow *window;
public:
	explicit Agent(NodeLauncher<Agent> &launcher, QObject *parent = nullptr);
	virtual ~Agent();

	virtual QWidget *showWindow();

public slots:
	void onConnectionStatusChanged(TryToggleState);


};



#endif // AGENT_HPP
