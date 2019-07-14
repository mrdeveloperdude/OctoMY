#include "HubMain.hpp"

// Needed to facilitate cast in AppLauncer
#include "node/NodeWindow.hpp"

#include "app/launcher/AppLauncher.hpp"

//#include "app/launcher/MockApp.hpp"
#include "hub/Hub.hpp"

#include "uptime/SharedPointerWrapper.hpp"

int main(int argc, char *argv[])
{
	int ret=EXIT_FAILURE;
	{
		QSharedPointer<AppLauncher<Hub> > hubMain=QSharedPointer<AppLauncher<Hub> >(OC_NEW AppLauncher<Hub>());
		if(!hubMain.isNull()) {
			hubMain->configure("hub", argc, argv);
			ret=hubMain->run();
			hubMain.clear();
		} else {
			qWarning()<<"ERROR: Could not allocate launcher";
		}
	}
	exit(ret);
}


