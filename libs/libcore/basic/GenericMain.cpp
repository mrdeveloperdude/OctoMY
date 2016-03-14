#include "GenericMain.hpp"

#include "comms/CommsChannel.hpp"
#include "basic/LogHandler.hpp"

#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QFileInfo>
#include <QSurfaceFormat>

GenericMain::GenericMain(int argc, char *argv[]):
	QObject(0)
  , sm(0)
  , ret(EXIT_SUCCESS)
  , argc(argc)
  , argv(argv)
  , app(0)
{
}

void GenericMain::run(){
	qsrand(QDateTime::currentMSecsSinceEpoch());
	setObjectName("GenericMain");
	LogHandler::setLogging(true);
	parser.setApplicationDescription("Robust real-time communication and control software for robots");
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
	headless=parser.isSet(headlessOption);

	app=(headless?(new QCoreApplication(argc, argv)):(new QApplication(argc, argv)));
	qDebug()<<(headless?"HEADLESS":"GUI ENABLED");
	start();


	if(0!=app){

		QSurfaceFormat fmt;
		fmt.setDepthBufferSize(24);
		/*
		if (QCoreApplication::arguments().contains(QStringLiteral("--multisample")))
			fmt.setSamples(4);
		if (QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"))) {
			fmt.setVersion(3, 2);
			fmt.setProfile(QSurfaceFormat::CoreProfile);
		}*/
		QSurfaceFormat::setDefaultFormat(fmt);

		Q_INIT_RESOURCE(style);
		Q_INIT_RESOURCE(fonts);
		Q_INIT_RESOURCE(icons);
		Q_INIT_RESOURCE(qfi);
		Q_INIT_RESOURCE(3d);


		QTimer::singleShot(0, this, SLOT(inLoop()));
		ret=app->exec();
		qDebug()<<QFileInfo( QCoreApplication::applicationFilePath()).fileName() << " done, quitting";
	}
	else{
		qWarning()<<"ERROR: no app, quitting";
	}
	stop();
}

GenericMain::~GenericMain(){

}

void GenericMain::inLoop(){
	sm=new StyleManager;
	if(0!=sm){
		sm->apply();
	}

}

