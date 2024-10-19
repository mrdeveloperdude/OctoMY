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
#include "node/NodeWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QProcessEnvironment>
#include <QSurfaceFormat>
#include <QWidget>
#include <qabstracteventdispatcher.h>

class Settings;

/** The AppLauncher class wraps an app main class providing it with all the
 *  necessary services that are important before and after the main eventloop
 *  of the application runs which is to say before and after qApp.exec()
 *
 *  This includes:
 *  + Parsing of commandline parameters
 *  + Maintaining environment variables
 *  + Selecting QCodeApplication or QApplication for headless or gui application
 *    respectively
 *  + Any OS level signal/event handling
 *  + Any OS level privilege stuff
 *  + Any OS Level watchdog stuff
 *  + Any OS level whatever!
 *  + Capturing and cleanup in case of exceptions and OS level lifecycle events
 *    such as sleep/termination etc.
 *  + API for graceful termination
 
A common application launch looks roughly like this (using Agent as example):

AppLauncher constructed in main()
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

... And application is running!

A common application shutdown looks roughly like this (using Agent as example):

AppLauncher nodeRequestClose()
AppLauncher appActivate(false)
Node appActivate(false)
Agent nodeActivate(false)
AppLauncher nodeActivateChanged(false)
AgentWindow closeEvent()
AppLauncher<T>::run() returns exit code to main()

... And application returns to O/S
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
	// Show debug output
	bool mDebug{false};
	
	
private:
	QMetaObject::Connection nodeActivateChangedConnection;
	QMetaObject::Connection nodeRequestedExitConnection;
	
public:
	explicit AppLauncher();
	virtual ~AppLauncher() override;
	
public:
	// Configure this app launcher before it is started. This version has no arguments
	void configure(QString base);
	// Configure this app launcher before it is started. This version accepts classic arguments
	void configure(QString base, int argc, char *argv[]);
	// Configure this app launcher before it is started. This version accepts context object directly
	void configure(QSharedPointer<AppContext> context);
	
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
	void appActivateDone(const bool on) override;
	
	// Provide application context
	QSharedPointer<AppContext> context() override;
	
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
void AppLauncher<T>::configure(QString base)
{
	//qDebug()<<"configure()";
	OC_METHODGATE();
	configure(base, 0, nullptr);
}


template <typename T>
void AppLauncher<T>::configure(QString base, int argc, char *argv[])
{
	//qDebug()<<"configure()";
	OC_METHODGATE();
	if(mAppConfigureHelper.configure()) {
		auto clp = QSharedPointer <AppCommandLineParser>::create();
		if(!clp.isNull()) {
			clp->process(argc, argv);
			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
			mContext = QSharedPointer<AppContext>::create(clp, env, base, clp->isHeadless());
		}
	}
}


template <typename T>
void AppLauncher<T>::configure(QSharedPointer<AppContext> context)
{
	//qDebug()<<"configure()";
	OC_METHODGATE();
	if(mAppConfigureHelper.configure()) {
		mContext=context;
	}
}


template <typename T>
int AppLauncher<T>::run()
{
	OC_METHODGATE();
	//qDebug()<<"run()";
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);
	if(!mContext.isNull()) {
		AppRenderingSettingsProvider asfp;
		QSurfaceFormat::setDefaultFormat(asfp.surfaceFormat());
		asfp.applyApplicationAttributes();
		int argc = mContext->commandLine()->argc();
		char **argv = mContext->commandLine()->argv();
		//qDebug()<<(mContext->isHeadless()?"HEADLESS":"HEADFULL");
		const auto lastAppInstance = QCoreApplication::instance();
		bool alreadyRunning{false};
		if(lastAppInstance){
			if(mDebug){
				qDebug() << "Found existing app instance";
			}
			mQApp = lastAppInstance;
			// We assume it is already running
			alreadyRunning = true;
		}
		else {
			if(mDebug){
				qDebug() << "Allocating new app instance";
			}
			mQApp = (mContext->isHeadless()?(OC_NEW QCoreApplication(argc, argv)):(OC_NEW QApplication(argc, argv)));
		}
		if(nullptr != mQApp) {
			// We need full control of when application quits
			QApplication::setQuitOnLastWindowClosed(false);
			appActivate(true);
			//RESOURCES HERE
			if(mDebug){
				qDebug() << QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " starting ";
			}
			// Start Qt event loop for application.
			// NOTE: This call will block until mQApp::quit()  mQApp::exit() is called and the eventloop is terminated
			if(! alreadyRunning){
				mReturnValue = mQApp->exec();
			}
			else{
				mReturnValue = 0;
			}
			if(mDebug){
				qDebug() << QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done with "<< mReturnValue << ", quitting";
			}
		} else {
			qWarning() << "ERROR: no app, quitting";
			mReturnValue = EXIT_FAILURE;
		}
	} else {
		qWarning() << "ERROR: No context";
	}
	return mReturnValue;
}


template <typename T>
void AppLauncher<T>::appActivate(const bool on)
{
	OC_METHODGATE();
	//qDebug()<<"appActivate(on="<<on<<")";
	if(on) {
		if(mAppConfigureHelper.activate(on)) {
			if(!mContext.isNull()) {
				mApp = QSharedPointer<T>::create();
				if(!mApp.isNull()) {
					T *appPtr = mApp.data();
					Q_ASSERT(nullptr != appPtr);
					auto sharedThis = AppLauncher<T>::sharedFromThis();
					if(!sharedThis.isNull()) {
						mApp->appConfigure(sharedThis);
						
						if(nodeActivateChangedConnection){
							qWarning() << "Trying to connect duplicate nodeActivateChangedConnection";
						}
						else{
							// Handle when (de)activation of this app is completed
							nodeActivateChangedConnection = QObject::connect(appPtr, &T::nodeActivateChanged, appPtr, [=](const bool on) {
								if(mDebug){
									qDebug() << "nodeActivationChanged(on=" << on << ")";
								}
								appActivateDone(on);
							});
						}
						
						if(nodeRequestedExitConnection){
							qWarning() << "Trying to connect duplicate nodeRequestedExitConnection";
						}
						else{
							// Handle when someone wants the app to stop
							nodeRequestedExitConnection = QObject::connect(appPtr, &T::nodeRequestExit, appPtr, [=](const int returnValue) {
								if(mDebug){
									qDebug() << "nodeRequestExit(" << returnValue << ")";
								}
								mReturnValue = returnValue;
								appPtr->appActivate(false);
							});
						}
						// Run the node's appActivate() from eventloop since it will call pure virtual members (which as you know are NOT allowed to be run from ctor in C++)
						QTimer::singleShot(0, Qt::VeryCoarseTimer, [appPtr]() {
							appPtr->appActivate(true);
						});
					} else {
						qWarning() << "ERROR: No launcher";
					}
				} else {
					qWarning() << "ERROR: No node could be created";
				}
			} else {
				qWarning() << "ERROR: No context";
			}
		}
	} else {
		if(mAppConfigureHelper.isActivatedAsExpected()) {
			if(!mApp.isNull()) {
				//mApp->appActivate(on);
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
				mWindow=qSharedPointerCast<NodeWindow>(mApp->appWindow());
				if(!mWindow.isNull()) {
					mWindow->show();
				} else {
					qWarning() <<"ERROR: No window to show";
				}
			}
		} else {
			//qDebug()<<"Closing window";
			if(!mWindow.isNull()) {
				mWindow->close();
				mWindow.clear();
			}
			if(mDebug){
				qDebug() << "Closing app";
			}
			if(!mApp.isNull()) {
				mApp.clear();
			}
			if(nullptr!=mQApp) {
				if(mDebug){
					qDebug() << "CALLING EXIT WITH " << mReturnValue;
				}
				mQApp->exit(mReturnValue);
			} else {
				qWarning() << "ERROR: No mQApp to exit";
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

