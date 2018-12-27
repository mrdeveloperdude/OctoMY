#ifndef NODELAUNCHER_HPP
#define NODELAUNCHER_HPP

#include "uptime/MethodGate.hpp"
#include "IAppLauncher.hpp"

/*
#include "basic/StyleManager.hpp"
#include "glt/IncludeOpenGL.hpp"
#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"

#include "comms/CommsChannel.hpp"
#include "basic/LogHandler.hpp"
*/

#include <QApplication>
#include <QSharedPointer>
/*
#include <QObject>

#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QFileInfo>
#include <QSurfaceFormat>
*/

class Settings;

/** The AppLauncher class wraps a an app main class providing it with all the necessary services that are important before and after the main eventloop of the application runs
 *  which is to say before and after qApp.exec()
 *
 *  This includes:
 *  + Parsing of commandline parameters
 *  + Maintaining environment variables
 *  + Selecting QCodeApplication or QApplication for headless or gui application respectively
 *  + Any OS level signal/event handling
 *  + Any OS level privilege stuff
 *  + Any OS Level watchdog stuff
 *  + Ant OS level whatever!
 *  + Capturing and cleanup in case of exceptions and OS level lifecycle events such as sleep/termination etc.
 *  + API for graceful termination
 *
 */

template <typename T>
class AppLauncher: public IAppLauncher
{
private:
	QSharedPointer<T> mApp;
	QSharedPointer<QWidget> mWindow;
	StyleManager *mStyleManager;
	int mReturnValue;
	int mArgc;
	char **mArgv;
	QCoreApplication *mQApp;

protected:
	QCommandLineParser mCommandlineOptions;
	QProcessEnvironment mEnvironment;
	bool mIsHeadless;

public:
	explicit AppLauncher(int argc=0, char *argv[]=nullptr);
	virtual ~AppLauncher();

	int run();

	void start();
	void stop();

	QSharedPointer<T> app();

	// IAppLauncher interface
	virtual void appDone() Q_DECL_OVERRIDE;

	QCommandLineParser commandLine();

	QProcessEnvironment environment();

};


template <typename T>
AppLauncher<T>::AppLauncher(int argc, char *argv[])
	: mReturnValue(EXIT_SUCCESS)
	, mArgc(argc)
	, mArgv(argv)
	, mQApp(nullptr)
	, mEnvironment(QProcessEnvironment::systemEnvironment())
	, mIsHeadless(true)
{
	OC_METHODGATE();
}

template <typename T>
int AppLauncher<T>::run()
{
	OC_METHODGATE();
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);

	QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

	qsrand(utility::currentMsecsSinceEpoch<quint64>());

#ifndef Q_OS_ANDROID
	LogHandler::setLogging(true);
#endif

	mCommandlineOptions.setApplicationDescription("Robust real-time communication and control software for robots");
	mCommandlineOptions.addHelpOption();
	mCommandlineOptions.addVersionOption();

	QCommandLineOption localHostOption(QStringList() <<  "l" << "local-host", QCoreApplication::translate("main", "Select server host to listen."), QCoreApplication::translate("main", "local-host"));
	mCommandlineOptions.addOption(localHostOption);

	QCommandLineOption localPortOption(QStringList() <<  "p" << "local-port", QCoreApplication::translate("main", "Select server port to listen."), QCoreApplication::translate("main", "local-port"));
	mCommandlineOptions.addOption(localPortOption);

	QCommandLineOption remoteHostOption(QStringList() <<  "r" << "remote-host", QCoreApplication::translate("main", "Select remote host to target."), QCoreApplication::translate("main", "remote-host"));
	mCommandlineOptions.addOption(remoteHostOption);

	QCommandLineOption remotePortOption(QStringList() <<  "o" << "remote-port", QCoreApplication::translate("main", "Select remote port to target."), QCoreApplication::translate("main", "remote-port"));
	mCommandlineOptions.addOption(remotePortOption);

	QCommandLineOption headlessOption(QStringList() <<  "h" << "head-less", QCoreApplication::translate("main", "Don't display GUI"), QCoreApplication::translate("main", "head-less"));
	mCommandlineOptions.addOption(headlessOption);

	// Process the actual command line arguments given by the user
	QStringList arguments;
	for(int i=0; i<mArgc; ++i) {
		arguments<<mArgv[i];
	}
	mCommandlineOptions.process(arguments);
	mIsHeadless=mCommandlineOptions.isSet(headlessOption);

	QSurfaceFormat format=QSurfaceFormat::defaultFormat();
	format.setVersion( OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR );
	format.setProfile( QSurfaceFormat::OCTOMY_QT_OGL_SURFACE_PROFILE );
	format.setOption(QSurfaceFormat::DebugContext);
	format.setDepthBufferSize(OCTOMY_QT_OGL_DEPTH_BUFFER);
	format.setStencilBufferSize(OCTOMY_QT_OGL_STENSIL_BUFFER);
	format.setSwapBehavior(QSurfaceFormat::OCTOMY_QT_OGL_SWAP_BEHAVIOUR);
	format.setSwapInterval(OCTOMY_QT_OGL_SWAP_INTERVAL);
	format.setRenderableType(QSurfaceFormat::OCTOMY_QT_OGL_RENDERABLE_TYPE);
	QSurfaceFormat::setDefaultFormat(format);
	QApplication::setAttribute(Qt::OCTOMY_QT_OGL_APP_ATTRIBUTE);
	QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

	mQApp=(mIsHeadless?(OC_NEW QCoreApplication(mArgc, mArgv)):(OC_NEW QApplication(mArgc, mArgv)));
	//qDebug()<<(mIsHeadless?"HEADLESS":"GUI ENABLED");

	if(nullptr!=mQApp) {
		QApplication::setQuitOnLastWindowClosed(false);
		start();
		Q_INIT_RESOURCE(icons);
		Q_INIT_RESOURCE(images);
		Q_INIT_RESOURCE(3d);

		mReturnValue=mQApp->exec();

		qDebug()<<QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done, quitting";
	} else {
		qWarning()<<"ERROR: no app, quitting";
	}
	return mReturnValue;
}


template <typename T>
void AppLauncher<T>::start()
{
	OC_METHODGATE();
	//qDebug()<<"NODE LAUNCHER: START";
	mApp=QSharedPointer<T>(OC_NEW T(*this, nullptr));
	if(!mApp.isNull()) {
		if(!mIsHeadless) {
			QObject::connect(mApp.data(), &T::appClose, mApp.data(), [=]() {
				//qDebug()<<"QUIT WELL RECIEVED";
				stop();
			});
			QObject::connect(mApp.data(), &T::appLoaded, mApp.data(), [=]() {
				//qDebug()<<"LOAD WELL RECIEVED";
				mWindow=mApp->showWindow();
			});
		}
	}
	else{
		qWarning()<<"ERROR: No node could be created";
	}
}


template <typename T>
void AppLauncher<T>::stop()
{
	OC_METHODGATE();
	//qDebug()<<"NODE LAUNCHER: STOP";
	if(!mApp.isNull()) {
		//mNode.reset();
		mApp->deInit();
		mApp->deleteLater();
		mApp=nullptr;
	}
}

template <typename T>
QSharedPointer<T> AppLauncher<T>::app()
{
	OC_METHODGATE();
	return mApp;
}

template <typename T>
void AppLauncher<T>::appDone()
{
	OC_METHODGATE();
	//qDebug()<<"NODELAUNCHER DONE";
	if(nullptr!=mQApp) {
		mQApp->quit();
	}
}


template <typename T>
AppLauncher<T>::~AppLauncher()
{
	OC_METHODGATE();
}

template <typename T>
QCommandLineParser AppLauncher<T>::commandLine()
{
	OC_METHODGATE();
	return mCommandlineOptions;
}

template <typename T>
QProcessEnvironment AppLauncher<T>::environment()
{
	OC_METHODGATE();
	return mEnvironment;
}


#endif // NODELAUNCHER_HPP

