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


	// CommsChannel slots
private slots:
	virtual void onCommsError(QString) Q_DECL_OVERRIDE;
	virtual void onCommsClientAdded(Client *)Q_DECL_OVERRIDE;
	virtual void onCommsConnectionStatusChanged(bool)Q_DECL_OVERRIDE;



};



#endif // AGENT_HPP
