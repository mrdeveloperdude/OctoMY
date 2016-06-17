#include "RemoteMain.hpp"

#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"



int main(int argc, char *argv[]){
	NodeLauncher<Remote> m(argc,argv);
	m.run();
}


