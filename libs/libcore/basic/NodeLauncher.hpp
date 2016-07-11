#ifndef NODELAUNCHER_HPP
#define NODELAUNCHER_HPP



#include <QObject>
#include <QApplication>
#include <QCommandLineParser>
#include <QProcessEnvironment>

#include "../libcore/basic/StyleManager.hpp"
#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"

class Node;
class Settings;


template <typename T>
class NodeLauncher{

	private:

		T *node;
		QWidget *window;

		StyleManager *style;
		int ret;
		int argc;
		char **argv;
		QCoreApplication *app;

	protected:
		QCommandLineParser opts;
		QProcessEnvironment env;
		bool headless;

	public:
		explicit NodeLauncher(int argc, char *argv[]);
		virtual ~NodeLauncher();

		void run();

		void start();
		void stop();

		QCommandLineParser &getOptions(){
			return opts;
		}

		QProcessEnvironment &getEnvironment(){
			return env;
		}

};

#include "comms/CommsChannel.hpp"
#include "basic/LogHandler.hpp"

#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QFileInfo>
#include <QSurfaceFormat>

template <typename T>
NodeLauncher<T>::NodeLauncher(int argc, char *argv[])
	: node(nullptr)
	, window(nullptr)
	, ret(EXIT_SUCCESS)
	, argc(argc)
	, argv(argv)
	, app(nullptr)
	, env(QProcessEnvironment::systemEnvironment())
	, headless(true)
{
}

template <typename T>
void NodeLauncher<T>::run(){
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);

	qsrand(QDateTime::currentMSecsSinceEpoch());


	LogHandler::setLogging(true);

	opts.setApplicationDescription("Robust real-time communication and control software for robots");
	opts.addHelpOption();
	opts.addVersionOption();

	QCommandLineOption localHostOption(QStringList() <<  "l" << "local-host", QCoreApplication::translate("main", "Select server host to listen."), QCoreApplication::translate("main", "local-host"));
	opts.addOption(localHostOption);

	QCommandLineOption localPortOption(QStringList() <<  "p" << "local-port", QCoreApplication::translate("main", "Select server port to listen."), QCoreApplication::translate("main", "local-port"));
	opts.addOption(localPortOption);

	QCommandLineOption remoteHostOption(QStringList() <<  "r" << "remote-host", QCoreApplication::translate("main", "Select remote host to target."), QCoreApplication::translate("main", "remote-host"));
	opts.addOption(remoteHostOption);

	QCommandLineOption remotePortOption(QStringList() <<  "o" << "remote-port", QCoreApplication::translate("main", "Select remote port to target."), QCoreApplication::translate("main", "remote-port"));
	opts.addOption(remotePortOption);

	QCommandLineOption headlessOption(QStringList() <<  "h" << "head-less", QCoreApplication::translate("main", "Don't display GUI"), QCoreApplication::translate("main", "head-less"));
	opts.addOption(headlessOption);

	// Process the actual command line arguments given by the user
	QStringList arguments;
	for(int i=0;i<argc;++i){
		arguments<<argv[i];
	}
	opts.process(arguments);
	headless=opts.isSet(headlessOption);

	app=(headless?(new QCoreApplication(argc, argv)):(new QApplication(argc, argv)));
	//qDebug()<<(headless?"HEADLESS":"GUI ENABLED");

	if(nullptr!=app){


		start();
		/*
		QSurfaceFormat fmt;
		fmt.setDepthBufferSize(24);

		if (QCoreApplication::arguments().contains(QStringLiteral("--multisample")))
			fmt.setSamples(4);
		if (QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"))) {
			fmt.setVersion(3, 2);
			fmt.setProfile(QSurfaceFormat::CoreProfile);
		}
		QSurfaceFormat::setDefaultFormat(fmt);
*/
		Q_INIT_RESOURCE(icons);
		Q_INIT_RESOURCE(images);
		Q_INIT_RESOURCE(qfi);
		Q_INIT_RESOURCE(3d);

		ret=app->exec();
		qDebug()<<QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done, quitting";
	}
	else{
		qWarning()<<"ERROR: no app, quitting";
	}
	stop();
}


template <typename T>
void NodeLauncher<T>::start(){
	node=new T(*this, nullptr);
	if(!headless && nullptr!=node){
		window=node->showWindow();
	}
}


template <typename T>
void NodeLauncher<T>::stop(){
	delete node;
	node=nullptr;
}

template <typename T>
NodeLauncher<T>::~NodeLauncher(){

}



#endif // NODELAUNCHER_HPP
