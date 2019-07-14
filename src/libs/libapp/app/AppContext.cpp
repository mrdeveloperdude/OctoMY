#include "AppContext.hpp"

#include "app/launcher/AppCommandLineParser.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QCoreApplication>
#include <QStandardPaths>


AppContext::AppContext(QSharedPointer<AppCommandLineParser> commandlineOptions, QProcessEnvironment environment, QString base, bool headless/*, QObject *parent*/)
	: mCommandlineOptions	(commandlineOptions)
	, mEnvironment			(environment)
	, mSettings				(OC_NEW Settings(base, Settings::APPLICATION_NAME_BASE+" "+base, Settings::APPLICATION_VERSION))
	, mBase					(base)
	, mBaseDir				(mSettings->getCustomSetting("content_dir", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)) )
	, mIsHeadless			(headless)
{
	OC_METHODGATE();
}

AppContext::~AppContext()
{
	OC_METHODGATE();
}


const QString AppContext::base() const
{
	OC_METHODGATE();
	return mBase;
}


const QString AppContext::baseDir() const
{
	OC_METHODGATE();
	return mBaseDir;
}


QSharedPointer <Settings> AppContext::settings()
{
	OC_METHODGATE();
	return mSettings;
}


const QSharedPointer<AppCommandLineParser> AppContext::commandLine() const
{
	OC_METHODGATE();
	return mCommandlineOptions;
}


const QProcessEnvironment AppContext::environment() const
{
	OC_METHODGATE();
	return mEnvironment;
}


bool AppContext::isHeadless() const
{
	OC_METHODGATE();
	return mIsHeadless;
}
