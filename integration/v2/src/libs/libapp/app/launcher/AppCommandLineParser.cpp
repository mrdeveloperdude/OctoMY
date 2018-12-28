#include "AppCommandLineParser.hpp"

#include "uptime/MethodGate.hpp"

#include <QCoreApplication>


AppCommandLineParser::AppCommandLineParser()
	: localHostOption(QStringList() <<  "l" << "local-host", QCoreApplication::translate("main", "Select server host to listen."), QCoreApplication::translate("main", "local-host"))
	, localPortOption(QStringList() <<  "p" << "local-port", QCoreApplication::translate("main", "Select server port to listen."), QCoreApplication::translate("main", "local-port"))
	, remoteHostOption(QStringList() <<  "r" << "remote-host", QCoreApplication::translate("main", "Select remote host to target."), QCoreApplication::translate("main", "remote-host"))
	, remotePortOption(QStringList() <<  "o" << "remote-port", QCoreApplication::translate("main", "Select remote port to target."), QCoreApplication::translate("main", "remote-port"))
	, headlessOption(QStringList() <<  "h" << "head-less", QCoreApplication::translate("main", "Don't display GUI"), QCoreApplication::translate("main", "head-less"))
{
	OC_METHODGATE();
	mCommandlineOptions.setApplicationDescription("OctoMY™ is the open-source, easy-to-use, state-of-the-art software that runs on your next robot.");
	mCommandlineOptions.addHelpOption();
	mCommandlineOptions.addVersionOption();
	mCommandlineOptions.addOption(localHostOption);
	mCommandlineOptions.addOption(localPortOption);
	mCommandlineOptions.addOption(remoteHostOption);
	mCommandlineOptions.addOption(remotePortOption);
	mCommandlineOptions.addOption(headlessOption);
}


// Process the actual command line arguments given by the user
void AppCommandLineParser::process(int argc, char **argv)
{
	OC_METHODGATE();
	QStringList arguments;
	for(int i=0; i<argc; ++i) {
		arguments<<argv[i];
	}
	// NOTE: This will terminate with exit() if an error is found in command-line arguments
	mCommandlineOptions.process(arguments);
}


bool AppCommandLineParser::isHeadless()
{
	OC_METHODGATE();
	return mCommandlineOptions.isSet(headlessOption);
}
