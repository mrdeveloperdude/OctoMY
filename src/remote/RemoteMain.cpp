#include "RemoteMain.hpp"

#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"



int main(int argc, char *argv[]){
	NodeLauncher<Remote> remoteMain(argc,argv);
	return remoteMain.run();
}


