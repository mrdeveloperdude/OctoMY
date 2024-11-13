#include "remote/RemoteMain.hpp"


/*!
	\fn int main(int argc, char *argv[])
	\brief Main application entry point for OctoMY Remote.

	\ingroup app

	\sa AgentMain, HubMain, ZooMain
*/


int main(int argc, char *argv[])
{
	RemoteMain remote(argc, argv);
	return 0;
}


