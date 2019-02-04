#include "AgentMain.hpp"

#include "app/launcher/AppLauncher.hpp"

//#include "app/launcher/MockApp.hpp"
#include "agent/Agent.hpp"

#include "uptime/SharedPointerWrapper.hpp"

int main(int argc, char *argv[])
{
	QSharedPointer<AppLauncher<Agent> > agentMain=QSharedPointer<AppLauncher<Agent> >(OC_NEW AppLauncher<Agent>());
	if(!agentMain.isNull()) {
		agentMain->configure(argc, argv, "agent");
		return agentMain->run();
	} else {
		qWarning()<<"ERROR: Could not allocate launcher";
	}
	return EXIT_FAILURE;
}


