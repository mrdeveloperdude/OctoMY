#include "HubMain.hpp"

#include "hub/Hub.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "uptime/SharedPointerWrapper.hpp"


HubMain::HubMain(int argc, char *argv[]){
	auto ret = EXIT_FAILURE;
	{
		auto hubMain = QSharedPointer<AppLauncher<Hub> >::create();
		if(!hubMain.isNull()) {
			hubMain->configure("hub", argc, argv);
			ret = hubMain->run();
			hubMain.clear();
		} else {
			qWarning()<<"ERROR: Could not allocate launcher";
		}
	}
	exit(ret);
}
