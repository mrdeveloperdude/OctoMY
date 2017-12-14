#include "HubMain.hpp"

#include "hub/Hub.hpp"


int main(int argc, char *argv[]){
	NodeLauncher<Hub> hubMain(argc,argv);
	return hubMain.run();
}
