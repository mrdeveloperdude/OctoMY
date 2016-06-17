#include "HubMain.hpp"

#include "hub/Hub.hpp"


int main(int argc, char *argv[]){
	NodeLauncher<Hub> m(argc,argv);
	m.run();
}
