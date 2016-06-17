#include "Hub.hpp"
#include "HubWindow.hpp"





Hub::Hub(NodeLauncher<Hub> &launcher, QObject *parent)
	: Node(launcher.getOptions(), "hub", parent)
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

