#include "agent/AgentMain.hpp"

/*!
	\fn int main(int argc, char *argv[])
	\brief Main application entry point for OctoMY Agent.

	\ingroup app

	\sa RemoteMain, HubMain, ZooMain
*/

int main(int argc, char *argv[])
{
	AgentMain agent(argc, argv);
	return 0;
}


