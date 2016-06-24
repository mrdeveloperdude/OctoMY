#include "Agent.hpp"
#include "AgentWindow.hpp"

#include "basic/Standard.hpp"

#include "comms/Client.hpp"
#include "comms/discovery/DiscoveryClient.hpp"
#include "zoo/ZooClient.hpp"

#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(launcher.getOptions(), "agent", ROLE_AGENT, TYPE_AGENT, parent)
	, window(nullptr)
{
}

Agent::~Agent(){
}



void Agent::start(QHostAddress listenAddress, quint16 listenPort, QHostAddress hubAddress, quint16 hubPort){

	this->hubAddress=hubAddress;
	this->hubPort=hubPort;
	if(nullptr!=comms){
		Client *c=comms->getClients()->getByHost(hubAddress,hubPort,true);
		if(0!=c){
			//	poseCourier->setDestination(c->signature);
			qDebug()<<"comms.start remote "<<listenAddress<<":"<<listenPort<<" -> hub "<<hubAddress.toString()<<":"<<hubPort<<"";
			comms->start(listenAddress,listenPort);
		}
		else{
			qWarning()<<"ERROR: could not get client for agent";
		}
	}
}




QWidget *Agent::showWindow(){
	if(nullptr==window){
		window=new AgentWindow(this, nullptr);
	}
	if(nullptr!=window){
		window->show();
	}
	return window;
}

void Agent::onConnectionStatusChanged(TryToggleState){

}
