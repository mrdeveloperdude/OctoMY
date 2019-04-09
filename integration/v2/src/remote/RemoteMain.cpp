#include "RemoteMain.hpp"


#include "app/launcher/AppLauncher.hpp"

#include "remote/Remote.hpp"


#include "uptime/SharedPointerWrapper.hpp"




int main(int argc, char *argv[])
{
	QSharedPointer<AppLauncher<Remote> > remoteMain=QSharedPointer<AppLauncher<Remote> >(OC_NEW AppLauncher<Remote>());
	int ret=EXIT_FAILURE;
	if(!remoteMain.isNull()) {
		remoteMain->configure(argc, argv, "remote");
		ret=remoteMain->run();
		remoteMain.clear();
	} else {
		qWarning()<<"ERROR: Could not allocate launcher";
	}
	exit(ret);
}

