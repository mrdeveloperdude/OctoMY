#include "AgentMain.hpp"

#include "agent/AgentWindow.hpp"
#include "agent/Agent.hpp"

AgentMain::AgentMain(int argc, char *argv[])
	: GenericMain(argc,argv)
	, agent(0)
	, window(0)
{

}

void AgentMain::start(){
	agent=new Agent(parser);
	if(!headless){
		window=new AgentWindow (agent,0);
		window->show();
	}
}


void AgentMain::stop(){
	delete window;
	window=0;
	delete agent;
	agent=0;
}




int main(int argc, char *argv[]){
	AgentMain m(argc,argv);
	m.run();
}


