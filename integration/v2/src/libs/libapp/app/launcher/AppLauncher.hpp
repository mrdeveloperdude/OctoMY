#ifndef APPLAUNCHER_HPP
#define APPLAUNCHER_HPP


#include "IAppLauncher.hpp"

#include "app/style/StyleManager.hpp"
#include "app/Settings.hpp"
#include "app/log/LogHandler.hpp"
#include "app/launcher/AppRenderingSettingsProvider.hpp"
#include "app/launcher/AppCommandLineParser.hpp"
#include "app/AppContext.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QApplication>
#include <QCoreApplication>
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
A common application launch looks roughly like this:


AppLauncher constructed
AppLauncher configure
AppLauncher run
AppLauncher appStart()
Node constructed
Agent constructed
Node appConfigure()
Agent nodeConfigure()
Node appInit()
Agent nodeInit()
AppLauncher nodeInitDone()
Node appWindow()
Agent nodeWindow()


... Application is running!


AppLauncher nodeRequestClose()
AppLauncher appStop()
Node appDeInit()
Agent nodeDeInit()
AppLauncher nodeDeInitDone()
AppLauncher appDeInitDone()
AgentWindow closeEvent()
AppLauncher<T>::run()

... Application returns to OS
*/


template <typename T>
class AppLauncher: public IAppLauncher, public QEnableSharedFromThis<AppLauncher<T> >
{
private:
	QSharedPointer<AppContext> mContext;
	QSharedPointer<T> mApp;
	QSharedPointer<QWidget> mWindow;
	QSharedPointer<StyleManager> mStyleManager;
	int mReturnValue;
	QCoreApplication *mQApp;

public:
	explicit AppLauncher();
	virtual ~AppLauncher() Q_DECL_OVERRIDE;

public:
	// Configure this app launcher before it is started
	void configure(int argc, char *argv[], QString base);

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
	// Provide the application instance
	QSharedPointer<T> app();

	// Provide the application context
	QSharedPointer<AppContext> context();

};


template <typename T>
AppLauncher<T>::AppLauncher()
// Default to exit with failure as a means to detect if something happens before we manage to exit cleanly
	: mReturnValue(EXIT_FAILURE)
	, mQApp(nullptr)
{
	OC_METHODGATE();
	qDebug()<<"AppLauncher()";
#ifndef Q_OS_ANDROID
	LogHandler::setLogging(true);
#endif

}

template <typename T>
AppLauncher<T>::~AppLauncher()
{
	OC_METHODGATE();
	qDebug()<<"~AppLauncher()";
}


template <typename T>
void AppLauncher<T>::configure(int argc, char *argv[], QString base)
{
	qDebug()<<"configure()";
	OC_METHODGATE();
	QSharedPointer <AppCommandLineParser> clp(OC_NEW AppCommandLineParser());
	if(!clp.isNull()) {
		clp->process(argc, argv);
		QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
		mContext=QSharedPointer<AppContext>(OC_NEW AppContext(clp, env, base, clp->isHeadless()));
	}
}



template <typename T>
int AppLauncher<T>::run()
{
	OC_METHODGATE();
	qDebug()<<"run()";
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);

	qsrand(static_cast<uint>(utility::time::currentMsecsSinceEpoch<quint64>()));

	if(!mContext.isNull()) {

		AppRenderingSettingsProvider asfp;
		QSurfaceFormat::setDefaultFormat(asfp.surfaceFormat());
		asfp.applyApplicationAttributes();
		int argc=mContext->commandLine()->argc();
		char **argv=mContext->commandLine()->argv();
		qDebug()<<(mContext->isHeadless()?"HEADLESS":"HEADFULL");
		mQApp=(mContext->isHeadless()?(OC_NEW QCoreApplication(argc, argv)):(OC_NEW QApplication(argc, argv)));
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
			mReturnValue=EXIT_FAILURE;
		}
	}
	return mReturnValue;
}


template <typename T>
void AppLauncher<T>::appStart()
{
	OC_METHODGATE();
	qDebug()<<"appStart()";
	mApp=QSharedPointer<T>(OC_NEW T(nullptr));
	if(!mApp.isNull()) {
		T *appPtr=mApp.data();
		Q_ASSERT(nullptr!=appPtr);
		auto sharedThis=AppLauncher<T>::sharedFromThis();
		if(!sharedThis.isNull()) {
			mApp->appConfigure(sharedThis);

			if(!mContext.isNull()) {
				// Handle when initialization of this app is completed
				if(!QObject::connect(appPtr, &T::nodeInitDone, appPtr, [=]() {
				qDebug()<<"nodeInitDone()";
					if(!mContext->isHeadless()) {

						mWindow=mApp->appWindow();
						if(!mWindow.isNull()) {
							mWindow->show();
						} else {
							qWarning()<<"ERROR: No window to show";
						}
					}
				}, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect";
				}
				// Handle when someone wants the app to stop
				if(!QObject::connect(appPtr, &T::nodeRequestClose, appPtr, [=]() {
				qDebug()<<"nodeRequestClose()";
					appStop();
				}, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect";
				}
				// Handle when de-initialization of this app is completed
				if(!QObject::connect(appPtr, &T::nodeDeInitDone, appPtr, [=]() {
				qDebug()<<"nodeDeInitDone()";
					appDeInitDone();
				}, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect";
				}
			}
			// Run init() from eventloop since it will call pure virtual members (which as you know are NOT allowed to be run from ctor in C++)
			QTimer::singleShot(0, Qt::VeryCoarseTimer, [appPtr]() {
				appPtr->appInit();
			});
		} else {
			qWarning()<<"ERROR: No launcher";
		}
	} else {
		qWarning()<<"ERROR: No node could be created";
	}
}


template <typename T>
void AppLauncher<T>::appStop()
{
	OC_METHODGATE();
	qDebug()<<"appStop()";
	if(!mApp.isNull()) {
		mApp->appDeInit();
	}
}


template <typename T>
void AppLauncher<T>::appDeInitDone()
{
	OC_METHODGATE();
	qDebug()<<"appDeInitDone()";
	if(!mWindow.isNull()) {
		mWindow->close();
		mWindow.clear();
	}
	if(nullptr!=mQApp) {
		mQApp->quit();
	}
}

template <typename T>
QSharedPointer<T> AppLauncher<T>::app()
{
	OC_METHODGATE();
	return mApp;
}

template <typename T>
QSharedPointer<AppContext> AppLauncher<T>::context()
{
	OC_METHODGATE();
	return mContext;
}


#endif
// APPLAUNCHER_HPP

