#include "ServerMain.hpp"
#include "basic/ServerWindow.hpp"
#include "comms/CommsChannel.hpp"

#include <QDebug>
#include <QTimer>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>



ServerMain::ServerMain(int argc, char *argv[]):
	QObject(0)
  , sm(0)
  , ret(EXIT_SUCCESS)
  , argc(argc)
  , argv(argv)
  , app(0)
{

	QCommandLineParser parser;
	parser.setApplicationDescription("Robust real-time communication and control library for robots");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption localHostOption(QStringList() <<  "l" << "local-host", QCoreApplication::translate("main", "Select server host to listen."), QCoreApplication::translate("main", "local-host"));
	parser.addOption(localHostOption);

	QCommandLineOption localPortOption(QStringList() <<  "p" << "local-port", QCoreApplication::translate("main", "Select server port to listen."), QCoreApplication::translate("main", "local-port"));
	parser.addOption(localPortOption);

	QCommandLineOption remoteHostOption(QStringList() <<  "r" << "remote-host", QCoreApplication::translate("main", "Select remote host to target."), QCoreApplication::translate("main", "remote-host"));
	parser.addOption(remoteHostOption);

	QCommandLineOption remotePortOption(QStringList() <<  "o" << "remote-port", QCoreApplication::translate("main", "Select remote port to target."), QCoreApplication::translate("main", "remote-port"));
	parser.addOption(remotePortOption);

	QCommandLineOption headlessOption(QStringList() <<  "h" << "head-less", QCoreApplication::translate("main", "Don't display GUI"), QCoreApplication::translate("main", "head-less"));
	parser.addOption(headlessOption);

	// Process the actual command line arguments given by the user
	QStringList arguments;
	for(int i=0;i<argc;++i){
		arguments<<argv[i];
	}
	parser.process(arguments);

	//TODO: Enforce headless throughout code, not just here
	app=createApplication(argc,argv,!parser.isSet(headlessOption));

	const QStringList args = parser.positionalArguments();
	// source is args.at(0), destination is args.at(1)

	CommsChannel *server=new CommsChannel();

	ServerWindow w(parser,server);
	server->setLogOutput(&w);
	w.show();


	Q_INIT_RESOURCE(style);
	Q_INIT_RESOURCE(fonts);
	Q_INIT_RESOURCE(icons);
	if(0!=app){
		QTimer::singleShot(0, this, SLOT(run()));
		ret=app->exec();
	}
	else{
		qWarning()<<"ERROR: no app, quitting";
	}
}


QCoreApplication* ServerMain::createApplication(int &argc, char *argv[], bool useGUI){
	if(useGUI){
		//qDebug()<<"GUI ENABLED";
		return new QApplication(argc, argv);
	}
	else{
		//qDebug()<<"HEADLESS";
		return new QCoreApplication(argc, argv);
	}
}


ServerMain::~ServerMain(){
}


void ServerMain::run(){
	sm=new StyleManager;
	if(0!=sm){
		sm->apply();
	}
}


int main(int argc, char *argv[]){
	ServerMain m(argc,argv);
}



