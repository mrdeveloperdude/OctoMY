#ifndef NODELAUNCHER_HPP
#define NODELAUNCHER_HPP

#include <QObject>
#include <QApplication>
#include <QCommandLineParser>
#include <QProcessEnvironment>


#include "basic/StyleManager.hpp"
#include "glt/IncludeOpenGL.hpp"
#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"

#include "comms/CommsChannel.hpp"
#include "basic/LogHandler.hpp"

#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QFileInfo>
#include <QSurfaceFormat>

class Node;
class Settings;


template <typename T>
class NodeLauncher
{
private:
	T *mNode;
	QWidget *mWindow;
	StyleManager *mStyleManager;
	int mReturnValue;
	int mArgc;
	char **mArgv;
	QCoreApplication *mApp;

protected:
	QCommandLineParser mCommandlineOptions;
	QProcessEnvironment mEnvironment;
	bool mIsHeadless;

public:
	explicit NodeLauncher(int argc, char *argv[]);
	virtual ~NodeLauncher();

	void run();

	void start();
	void stop();

	QCommandLineParser &options();

	QProcessEnvironment &environment();

};


template <typename T>
NodeLauncher<T>::NodeLauncher(int argc, char *argv[])
	: mNode(nullptr)
	, mWindow(nullptr)
	, mReturnValue(EXIT_SUCCESS)
	, mArgc(argc)
	, mArgv(argv)
	, mApp(nullptr)
	, mEnvironment(QProcessEnvironment::systemEnvironment())
	, mIsHeadless(true)
{
	OC_METHODGATE();
}

template <typename T>
void NodeLauncher<T>::run()
{
	OC_METHODGATE();
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);

	QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

	qsrand(QDateTime::currentMSecsSinceEpoch());

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

	mApp=(mIsHeadless?(OC_NEW QCoreApplication(mArgc, mArgv)):(OC_NEW QApplication(mArgc, mArgv)));
	//qDebug()<<(headless?"HEADLESS":"GUI ENABLED");

	if(nullptr!=mApp) {


		start();

		QSurfaceFormat format=QSurfaceFormat::defaultFormat();
		format.setVersion( OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR );
		format.setProfile( QSurfaceFormat::OCTOMY_QT_OGL_SURFACE_PROFILE );
		format.setRenderableType( QSurfaceFormat::OpenGL);
		format.setOption(QSurfaceFormat::DebugContext);
		format.setDepthBufferSize(24);

		format.setStencilBufferSize(0);
		format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
		format.setSwapInterval(1);
		QSurfaceFormat::setDefaultFormat(format);

		Q_INIT_RESOURCE(icons);
		Q_INIT_RESOURCE(images);
		Q_INIT_RESOURCE(3d);

		mReturnValue=mApp->exec();
		qDebug()<<QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done, quitting";
	} else {
		qWarning()<<"ERROR: no app, quitting";
	}
	stop();
}


template <typename T>
void NodeLauncher<T>::start()
{
	OC_METHODGATE();
	mNode=OC_NEW T(*this, nullptr);
	if(!mIsHeadless && nullptr!=mNode) {
		mWindow=mNode->showWindow();
	}
}


template <typename T>
void NodeLauncher<T>::stop()
{
	OC_METHODGATE();
	if(nullptr!=mNode) {
		mNode->deleteLater();
		mNode=nullptr;
	}
}

template <typename T>
NodeLauncher<T>::~NodeLauncher()
{
	OC_METHODGATE();
}

template <typename T>
QCommandLineParser &NodeLauncher<T>::options()
{
	OC_METHODGATE();
	return mCommandlineOptions;
}

template <typename T>
QProcessEnvironment &NodeLauncher<T>::environment()
{
	OC_METHODGATE();
	return mEnvironment;
}


#endif // NODELAUNCHER_HPP

