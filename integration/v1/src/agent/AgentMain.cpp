#include "AgentMain.hpp"

#include "agent/Agent.hpp"

int main(int argc, char *argv[])
{
	NodeLauncher<Agent> agentMain(argc,argv);
	return agentMain.run();
}


