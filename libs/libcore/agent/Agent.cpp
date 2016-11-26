#include "Agent.hpp"
#include "AgentWindow.hpp"

#include "../libutil/utility/Standard.hpp"

#include "comms/Client.hpp"
#include "comms/ClientDirectory.hpp"

#include "discovery/DiscoveryClient.hpp"
#include "zoo/ZooClient.hpp"
#include "basic/AppContext.hpp"


#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(new AppContext(launcher.getOptions(), launcher.getEnvironment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, window(nullptr)
{
}

Agent::~Agent()
{
}




QWidget *Agent::showWindow()
{
	if(nullptr==window) {
		window=new AgentWindow(this, nullptr);
	}
	if(nullptr!=window) {
		window->show();
	}
	return window;
}





//////////////////////////////////////////////////
// CommsChannel slots


void Agent::onCommsError(QString e)
{
	//qDebug()<<"AGENT UNIMP Comms error: "<<e;
}

void Agent::onCommsClientAdded(Client *c)
{
	//qDebug()<<"AGENT UNIMP Client added: "<<c->toString();
}

void Agent::onCommsConnectionStatusChanged(bool s)
{
	//qDebug() <<"AGENT UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}
