#include "Hub.hpp"
#include "HubWindow.hpp"

#include "node/AppContext.hpp"



Hub::Hub(NodeLauncher<Hub> &launcher, QObject *parent)
	: Node(OC_NEW AppContext(launcher.getOptions(), launcher.getEnvironment(), "hub", parent), ROLE_CONTROL, TYPE_HUB, parent)
	, window(nullptr)
{
}

Hub::~Hub(){
}



QWidget *Hub::showWindow(){
	if(nullptr==window){
		window=OC_NEW HubWindow(this, nullptr);
	}
	if(nullptr!=window){
		window->show();
	}
	return window;
}

