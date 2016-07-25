#include "ZooMain.hpp"
#include "zoo/ZooServer.hpp"
#include "basic/LogHandler.hpp"
#include "basic/Settings.hpp"
#include "basic/AppContext.hpp"

#include <QCoreApplication>

#include <QCommandLineParser>
#include <QProcessEnvironment>


int main(int argc, char *argv[]){
	qsrand(QDateTime::currentMSecsSinceEpoch());
	LogHandler::setLogging(true);

	QCoreApplication app(argc, argv);

	//Q_INIT_RESOURCE(style);
	//Q_INIT_RESOURCE(fonts);
	Q_INIT_RESOURCE(icons);
	//Q_INIT_RESOURCE(qfi);
	//Q_INIT_RESOURCE(3d);

	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	const QString OCTOMY_ZOO_WEB_PORT=QStringLiteral("OCTOMY_ZOO_WEB_PORT");
	QString port="8123";
	if(env.contains(OCTOMY_ZOO_WEB_PORT)){
		port=env.value(OCTOMY_ZOO_WEB_PORT);
	}

	QCommandLineParser opts;

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

	ZooServer server(new AppContext(opts, env, "zoo", nullptr), nullptr);
	server.start(port);

	return app.exec();
}
