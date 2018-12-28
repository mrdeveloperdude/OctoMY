#ifndef NODELAUNCHER_HPP
#define NODELAUNCHER_HPP

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "IAppLauncher.hpp"

#include "app/style/StyleManager.hpp"
#include "app/Settings.hpp"
#include "app/log/LogHandler.hpp"
#include "app/launcher/AppRenderingSettingsProvider.hpp"
#include "app/launcher/AppCommandLineParser.hpp"

#include "utility/time/HumanTime.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QSharedPointer>
#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QLoggingCategory>
#include <QSurfaceFormat>
#include <QFileInfo>
#include <QWidget>


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


public:
	// The entry point. This is what gets called to use this app launcher after it has been created
	int run();


protected:

	// Internal helper to start app
	void appStart();

	// Internal helper to stop app
	void appStop();

	// Internal helper to handle termination of launcher after app completes
	void appDeInitDone() Q_DECL_OVERRIDE;

public:
	QSharedPointer<T> app();

	QCommandLineParser commandLine() const;
	QProcessEnvironment environment() const;

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

	qsrand(static_cast<uint>(utility::time::currentMsecsSinceEpoch<quint64>()));

#ifndef Q_OS_ANDROID
	LogHandler::setLogging(true);
#endif

	// Process the actual command line arguments given by the user
	AppCommandLineParser clp;
	clp.process(mArgc, mArgv);
	mIsHeadless=clp.isHeadless();
	AppRenderingSettingsProvider asfp;
	QSurfaceFormat::setDefaultFormat(asfp.surfaceFormat());
	QApplication::setAttribute(asfp.applicationAttributes());
	QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

	mQApp=(mIsHeadless?(OC_NEW QCoreApplication(mArgc, mArgv)):(OC_NEW QApplication(mArgc, mArgv)));
	//qDebug()<<(mIsHeadless?"HEADLESS":"GUI ENABLED");

	if(nullptr!=mQApp) {
		// We need full control of when application quits
		QApplication::setQuitOnLastWindowClosed(false);
		appStart();
		// Initialize resources here

		/*
		Q_INIT_RESOURCE(icons);
		Q_INIT_RESOURCE(images);
		Q_INIT_RESOURCE(3d);
		*/

		// Start Qt event loop for application.
		// NOTE: This call will block until mQApp::quit() is called and the eventloop is terminated
		mReturnValue=mQApp->exec();

		qDebug()<<QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done, quitting";
	} else {
		qWarning()<<"ERROR: no app, quitting";
		mReturnValue=1;
	}
	return mReturnValue;
}


template <typename T>
void AppLauncher<T>::appStart()
{
	OC_METHODGATE();
	qDebug()<<"appStart";
	mApp=QSharedPointer<T>(OC_NEW T(*this, nullptr));
	if(!mApp.isNull()) {
		T *appPtr=mApp.data();
		if(!mIsHeadless) {
			// Handle when initialization of this app (which is starte below) is completed
			QObject::connect(appPtr, &T::appInitDone, appPtr, [=]() {
				mWindow=mApp->appWindow();
				if(!mWindow.isNull()) {
					mWindow->show();
				}
			});
			// Handle when de-initialization of this app is completed
			QObject::connect(appPtr, &T::appDeInitDone, appPtr, [=]() {
				appDeInitDone();
			});
			// Handle when someone wants the app to stop
			QObject::connect(appPtr, &T::appRequestClose, appPtr, [=]() {
				appStop();
			});
		}
		mApp->appInit();
	} else {
		qWarning()<<"ERROR: No node could be created";
	}
}


template <typename T>
void AppLauncher<T>::appStop()
{
	OC_METHODGATE();
	qDebug()<<"appStop";
	if(!mApp.isNull()) {
		mApp->appDeInit();
	}
}


template <typename T>
void AppLauncher<T>::appDeInitDone()
{
	OC_METHODGATE();
	qDebug()<<"appDeInitDone";
	if(nullptr!=mQApp) {
		mQApp->quit();
	}
}

template <typename T>
AppLauncher<T>::~AppLauncher()
{
	OC_METHODGATE();
	qDebug()<<"AppLauncher d-tor";
}


template <typename T>
QSharedPointer<T> AppLauncher<T>::app()
{
	OC_METHODGATE();
	return mApp;
}



template <typename T>
QCommandLineParser AppLauncher<T>::commandLine() const
{
	OC_METHODGATE();
	return mCommandlineOptions;
}

template <typename T>
QProcessEnvironment AppLauncher<T>::environment() const
{
	OC_METHODGATE();
	return mEnvironment;
}


#endif // NODELAUNCHER_HPP

