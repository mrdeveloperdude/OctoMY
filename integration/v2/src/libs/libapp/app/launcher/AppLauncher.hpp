#ifndef APPLAUNCHER_HPP
#define APPLAUNCHER_HPP


#include "IAppLauncher.hpp"

#include "app/style/StyleManager.hpp"
#include "app/Settings.hpp"
#include "app/log/LogHandler.hpp"
#include "app/launcher/AppRenderingSettingsProvider.hpp"
#include "app/launcher/AppCommandLineParser.hpp"
#include "app/AppContext.hpp"
#include "app/Constants.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"

#include "uptime/SharedPointerWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

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
AppLauncher appActivate(true)
Node constructed
Agent constructed
Node appConfigure()
Agent nodeConfigure()
Node appActivate(true)
Agent nodeActivate(true)
AppLauncher nodeActivateDone(on)
Node appWindow()
Agent nodeWindow()


... Application is running!


AppLauncher nodeRequestClose()
AppLauncher appActivate(false)
Node appActivate(false)
Agent nodeActivate(false)
AppLauncher nodeActivateChanged(false)
AgentWindow closeEvent()
AppLauncher<T>::run()

... Application returns to OS
*/


template <typename T>
class AppLauncher: public IAppLauncher, public QEnableSharedFromThis<AppLauncher<T> >
{
private:

	// Helper to keep track of appConfigure() and appActivate() state
	ConfigureHelper mAppConfigureHelper;
	// The application context that contains such things as environment variables and command line arguments
	QSharedPointer<AppContext> mContext;
	// The aplication instance we are launching
	QSharedPointer<T> mApp;
	// The UI window for the application if it is not headless
	QSharedPointer<QWidget> mWindow;

	// The manager for UI styles
	QSharedPointer<StyleManager> mStyleManager;
	// The return value from main eventloop for application
	int mReturnValue;
	// The Qt application instance
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
	// Internal helper to start and stop app
	void appActivate(const bool on);

public:
	// Provide the application instance
	QSharedPointer<T> app();


	// IAppLauncher interface
public:
	// Called when app execution is done
	void appActivateDone(const bool on) Q_DECL_OVERRIDE;

	// Provide application context
	QSharedPointer<AppContext> context() Q_DECL_OVERRIDE;

};


template <typename T>
AppLauncher<T>::AppLauncher()
	: mAppConfigureHelper("launcher", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	  // Default to exit with failure as a means to detect if something happens before we manage to exit cleanly
	, mReturnValue(EXIT_FAILURE)
	, mQApp(nullptr)
{
	OC_METHODGATE();
	//qDebug()<<"AppLauncher()";
#ifndef Q_OS_ANDROID
	LogHandler::setLogging(true);
#endif

}

template <typename T>
AppLauncher<T>::~AppLauncher()
{
	OC_METHODGATE();
	mQApp=nullptr;
	//qDebug()<<"~AppLauncher()";
}


template <typename T>
void AppLauncher<T>::configure(int argc, char *argv[], QString base)
{
	//qDebug()<<"configure()";
	OC_METHODGATE();
	if(mAppConfigureHelper.configure()) {
		QSharedPointer <AppCommandLineParser> clp(OC_NEW AppCommandLineParser());
		if(!clp.isNull()) {
			clp->process(argc, argv);
			QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
			mContext=QSharedPointer<AppContext>(OC_NEW AppContext(clp, env, base, clp->isHeadless()));
		}
	}
}



template <typename T>
int AppLauncher<T>::run()
{
	OC_METHODGATE();
	//qDebug()<<"run()";
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);

	qsrand(static_cast<uint>(utility::time::currentMsecsSinceEpoch<quint64>()));

	if(!mContext.isNull()) {

		AppRenderingSettingsProvider asfp;
		QSurfaceFormat::setDefaultFormat(asfp.surfaceFormat());
		asfp.applyApplicationAttributes();
		int argc=mContext->commandLine()->argc();
		char **argv=mContext->commandLine()->argv();
		//qDebug()<<(mContext->isHeadless()?"HEADLESS":"HEADFULL");
		mQApp=(mContext->isHeadless()?(OC_NEW QCoreApplication(argc, argv)):(OC_NEW QApplication(argc, argv)));
		if(nullptr!=mQApp) {
			// We need full control of when application quits
			QApplication::setQuitOnLastWindowClosed(false);
			appActivate(true);
			// Initialize resources here

			/* TODO: Look at how to distribute this initialization wizely
			Q_INIT_RESOURCE(icons);
			Q_INIT_RESOURCE(images);
			Q_INIT_RESOURCE(3d);
			*/

			// Start Qt event loop for application.
			// NOTE: This call will block until mQApp::quit()  mQApp::exit() is called and the eventloop is terminated
			mReturnValue = mQApp->exec();

			//qDebug()<<QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done with "<< mReturnValue << ", quitting";
		} else {
			qWarning()<<"ERROR: no app, quitting";
			mReturnValue=EXIT_FAILURE;
		}
	} else {
		qWarning()<<"ERROR: No context";
	}
	return mReturnValue;
}


template <typename T>
void AppLauncher<T>::appActivate(const bool on)
{
	OC_METHODGATE();
	//qDebug()<<"appActivate(on="<<on<<")";
	if(mAppConfigureHelper.activate(on)) {
		if(on) {
			if(!mContext.isNull()) {
				mApp=QSharedPointer<T>(OC_NEW T());
				if(!mApp.isNull()) {
					T *appPtr=mApp.data();
					Q_ASSERT(nullptr!=appPtr);
					auto sharedThis=AppLauncher<T>::sharedFromThis();
					if(!sharedThis.isNull()) {
						mApp->appConfigure(sharedThis);
						// Handle when (de)activation of this app is completed
						if(!QObject::connect(appPtr, &T::nodeActivateChanged, appPtr, [=](const bool on) {
						//qDebug()<<"nodeActivationChanged(on="<<on<<")";
							appActivateDone(on);
						}, OC_CONTYPE)) {
							qWarning()<<"ERROR: Could not connect";
						}
						// Handle when someone wants the app to stop
						if(!QObject::connect(appPtr, &T::nodeRequestExit, appPtr, [=](const int returnValue) {
						//qDebug()<<"nodeRequestExit("<<returnValue<<")";
							mReturnValue=returnValue;
							appActivate(false);
						}, OC_CONTYPE)) {
							qWarning()<<"ERROR: Could not connect";
						}

						// Run the node's appActivate() from eventloop since it will call pure virtual members (which as you know are NOT allowed to be run from ctor in C++)
						QTimer::singleShot(0, Qt::VeryCoarseTimer, [appPtr]() {
							appPtr->appActivate(true);
						});
					} else {
						qWarning()<<"ERROR: No launcher";
					}
				} else {
					qWarning()<<"ERROR: No node could be created";
				}
			} else {
				qWarning()<<"ERROR: No context";
			}
		} else {
			if(!mApp.isNull()) {
				mApp->appActivate(on);
			}
		}
	}
}


template <typename T>
QSharedPointer<T> AppLauncher<T>::app()
{
	OC_METHODGATE();
	return mApp;
}


template <typename T>
void AppLauncher<T>::appActivateDone(const bool on)
{
	OC_METHODGATE();
	//qDebug()<<"appActivateDone(on="<<on<<")";
	if(!mContext.isNull()) {
		if(on) {
			//qDebug()<<"Opening window";
			if(!mContext->isHeadless()) {
				mWindow=mApp->appWindow();
				if(!mWindow.isNull()) {
					mWindow->show();
				} else {
					qWarning()<<"ERROR: No window to show";
				}
			}
		} else {
			//qDebug()<<"Closing window";
			if(!mWindow.isNull()) {
				mWindow->close();
				mWindow.clear();
			}
			//qDebug()<<"Closing app";
			if(!mApp.isNull()) {
				mApp.clear();
			}
			if(nullptr!=mQApp) {
				//qDebug()<<"CALLING EXIT WITH "<<mReturnValue;
				mQApp->exit(mReturnValue);
			} else {
				qWarning()<<"ERROR: No mQApp to exit";
			}
		}
	}
}



template <typename T>
QSharedPointer<AppContext> AppLauncher<T>::context()
{
	OC_METHODGATE();
	return mContext;
}




#endif
// APPLAUNCHER_HPP

