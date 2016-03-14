#include "HubMain.hpp"


#include "hub/HubWindow.hpp"
#include "hub/Hub.hpp"

HubMain::HubMain(int argc, char *argv[])
	: GenericMain(argc,argv)
	, hub(0)
	, window(0)
{

}

void HubMain::start(){
	hub=new Hub(parser);
	if(!headless){
		window=new HubWindow (hub,0);
		window->show();
	}
}


void HubMain::stop(){
	delete window;
	window=0;
	delete hub;
	hub=0;
}




int main(int argc, char *argv[]){
	HubMain m(argc,argv);
	m.run();
}
