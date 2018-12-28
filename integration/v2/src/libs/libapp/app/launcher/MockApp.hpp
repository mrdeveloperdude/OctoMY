#ifndef MOCKAPP_HPP
#define MOCKAPP_HPP

#include "app/launcher/AppLauncher.hpp"

#include <QObject>
#include <QSharedPointer>

class MockApp: public QObject
{
	Q_OBJECT

private:
	QSharedPointer<QWidget> mWindow;
public:

	explicit MockApp(AppLauncher<MockApp> &launcher, QObject *parent = nullptr);

	virtual ~MockApp();

public:
	// Called by launcher to give the app a chance to initialize
	// After calling this, the launcher will pause further initialization and showing of windows until app emits an "appLoaded" signal
	virtual void appInit();

	// Called by launcher as a response to an appRequestClose event being emitted by anyone that wants the app to close.
	// After calling this, the launcher will complete termination and return to OS
	virtual void appDeInit();

	// Called by launcher to get a handle to the app's main window
	// Will be called when launcher wants to show window during initialization
	// NOTE: Launcher will NOT hide the window, as it is expected that app should do this by itself upon de-initialization / destruction
	virtual QSharedPointer<QWidget> appWindow();


signals:
	// Expected to be emitted when someone wants app to terminate. Will be caught by app launcher and app launcher will subsequently call appDeInit() to bring termination to fruition
	void appRequestClose();

	// Expected to be emitted by app as soon as initialization is done in appInit();
	void appInitDone();

	// Expected to be emitted by app during termination as soon as it no longer needs the eventloop in appDeInit().
	void appDeInitDone();

};


#endif
// MOCKAPP_HPP
