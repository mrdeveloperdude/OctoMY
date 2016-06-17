#include "AgentMain.hpp"


#include "agent/Agent.hpp"



int main(int argc, char *argv[]){
	NodeLauncher<Agent> m(argc,argv);
	m.run();
}


