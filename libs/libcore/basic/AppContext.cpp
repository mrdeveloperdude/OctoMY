#include "AppContext.hpp"


#include <QCoreApplication>
#include <QStandardPaths>

AppContext::AppContext(QCommandLineParser &opts, QProcessEnvironment &env, QString base, QObject *parent)
	: QObject(parent)
	, mOpts (opts)
	, mEnv (env)
	, mSettings(base, Settings::APPLICATION_NAME_BASE+" "+base, Settings::APPLICATION_VERSION)
	, mBaseDir( mSettings.getCustomSetting("content_dir", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)) )

{
}
