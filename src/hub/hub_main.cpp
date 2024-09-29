#include "hub/HubMain.hpp"

/*!
	\fn int main(int argc, char *argv[])
	\brief Main application entry point for OctoMY Hub.

	\ingroup app

	\sa AgentMain, RemoteMain, ZooMain
*/

int main(int argc, char *argv[])
{
	HubMain hub(argc, argv);
	return 0;
}


