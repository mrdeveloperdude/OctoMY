#include "RemoteMain.hpp"

// Needed to facilitate cast in AppLauncer
#include "node/NodeWindow.hpp"

#include "app/launcher/AppLauncher.hpp"

//#include "app/launcher/MockApp.hpp"
#include "remote/Remote.hpp"

#include "uptime/SharedPointerWrapper.hpp"

int main(int argc, char *argv[])
{
	int ret=EXIT_FAILURE;
	{
		QSharedPointer<AppLauncher<Remote> > remoteMain=QSharedPointer<AppLauncher<Remote> >(OC_NEW AppLauncher<Remote>());
		if(!remoteMain.isNull()) {
			remoteMain->configure("remote", argc, argv);
			ret=remoteMain->run();
			remoteMain.clear();
		} else {
			qWarning()<<"ERROR: Could not allocate launcher";
		}
	}
	exit(ret);
}


