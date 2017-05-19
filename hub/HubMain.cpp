#include "HubMain.hpp"

#include "../libs/libhub/Hub.hpp"


int main(int argc, char *argv[]){
	NodeLauncher<Hub> m(argc,argv);
	m.run();
}
