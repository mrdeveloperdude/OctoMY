#include "AgentMain.hpp"

#include "agent/Agent.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "uptime/SharedPointerWrapper.hpp"


AgentMain::AgentMain(int argc, char *argv[]){
	auto ret = EXIT_FAILURE;
	{
		auto agentMain = QSharedPointer<AppLauncher<Agent> >::create();
		if(!agentMain.isNull()) {
			agentMain->configure("agent", argc, argv);
			ret = agentMain->run();
			agentMain.clear();
		} else {
			qWarning()<<"ERROR: Could not allocate launcher";
		}
	}
	exit(ret);
}

