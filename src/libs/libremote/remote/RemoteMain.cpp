#include "RemoteMain.hpp"


#include "remote/Remote.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "uptime/SharedPointerWrapper.hpp"


RemoteMain::RemoteMain(int argc, char *argv[]){
	auto ret = EXIT_FAILURE;
	{
		auto remoteMain = QSharedPointer<AppLauncher<Remote> >::create();
		if(!remoteMain.isNull()) {
			remoteMain->configure("remote", argc, argv);
			ret = remoteMain->run();
			remoteMain.clear();
		} else {
			qWarning()<<"ERROR: Could not allocate launcher";
		}
	}
	exit(ret);
}

