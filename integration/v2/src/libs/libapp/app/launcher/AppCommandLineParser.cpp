#include "AppCommandLineParser.hpp"

#include "uptime/MethodGate.hpp"

#include <QCoreApplication>


AppCommandLineParser::AppCommandLineParser()
	: mArgc(0)
	, mArgv(nullptr)
	, mParseDone(false)
	, mParseResult(false)
	, localHostOption(QStringList() <<  "l" << "local-host", QCoreApplication::translate("main", "Select server host to listen."), QCoreApplication::translate("main", "local-host"))
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


AppCommandLineParser::~AppCommandLineParser()
{
	OC_METHODGATE();
}

// Process the actual command line arguments given by the user
void AppCommandLineParser::process(int argc, char **argv)
{
	OC_METHODGATE();
	// We store these beacuse we need them later when calling QApplication (In AppLauncher)
	// NOTE: We assume that argv is allocated as long as the process is running and so we don't worry about memory management
	mArgc=argc;
	mArgv=argv;


	QStringList arguments;
	for(int i=0; i<argc; ++i) {
		arguments<<argv[i];
	}
	mParseResult=mCommandlineOptions.parse(arguments);
	mParseDone=true;
}


bool AppCommandLineParser::isParseDone()
{
	OC_METHODGATE();
	return mParseResult;
}


bool AppCommandLineParser::isParseOK()
{
	OC_METHODGATE();
	return mParseResult;
}


bool AppCommandLineParser::isHeadless()
{
	OC_METHODGATE();
	return mCommandlineOptions.isSet(headlessOption);
}


int AppCommandLineParser::argc()
{
	OC_METHODGATE();
	return mArgc;
}


char **AppCommandLineParser::argv()
{
	OC_METHODGATE();
	return mArgv;
}
