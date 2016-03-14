#include "RemoteMain.hpp"

#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"


RemoteMain::RemoteMain(int argc, char *argv[])
	: GenericMain(argc,argv)
	, remote(0)
	, window(0)
{

}

void RemoteMain::start(){
	remote=new Remote(parser);
	if(!headless){
		window=new RemoteWindow (remote,0);
		window->show();
	}
}


void RemoteMain::stop(){
	delete window;
	window=0;
	delete remote;
	remote=0;
}




int main(int argc, char *argv[]){
	RemoteMain m(argc,argv);
	m.run();
}


