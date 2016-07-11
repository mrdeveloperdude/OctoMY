#include "Hub.hpp"
#include "HubWindow.hpp"

#include "basic/AppContext.hpp"



Hub::Hub(NodeLauncher<Hub> &launcher, QObject *parent)
	: Node(new AppContext(launcher.getOptions(), launcher.getEnvironment(), "hub", this), ROLE_CONTROL, TYPE_HUB, parent)
	, window(nullptr)
{
}

Hub::~Hub(){
}



QWidget *Hub::showWindow(){
	if(nullptr==window){
		window=new HubWindow(this, nullptr);
	}
	if(nullptr!=window){
		window->show();
	}
	return window;
}

