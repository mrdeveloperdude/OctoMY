#include "ZooMain.hpp"
#include "zoo/ZooServer.hpp"

#include "log/LogHandler.hpp"

#include "app/AppContext.hpp"
#include "app/Constants.hpp"
#include "app/launcher/AppCommandLineParser.hpp"

#include <QCoreApplication>

#include <QCommandLineParser>
#include <QProcessEnvironment>


int main(int argc, char *argv[]){
	LogHandler::setLogging(true);

	QCoreApplication app(argc, argv);

	//Q_INIT_RESOURCE(style);
	//Q_INIT_RESOURCE(fonts);
	//Q_INIT_RESOURCE(icons);
	//Q_INIT_RESOURCE(qfi);
	//Q_INIT_RESOURCE(3d);

	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	const QString OCTOMY_ZOO_WEB_PORT=QStringLiteral("OCTOMY_ZOO_WEB_PORT");
	QString port=QString::number(Constants::OCTOMY_UDP_DEFAULT_PORT_ZOO);
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

	QCommandLineOption headlessOption(QStringList() <<  "e" << "head-less", QCoreApplication::translate("main", "Run in GUI-less environment"), QCoreApplication::translate("main", "head-less"));
	opts.addOption(headlessOption);

	// Process the actual command line arguments given by the user
	QStringList arguments;
	for(int i=0;i<argc;++i){
		arguments<<argv[i];
	}
	opts.process(arguments);

	const QString base="zoo";
	QSharedPointer<AppContext> ctx;

	auto clp = QSharedPointer <AppCommandLineParser>::create();
	if(!clp.isNull()) {
		clp->process(argc, argv);
		auto env = QProcessEnvironment::systemEnvironment();
		ctx = QSharedPointer<AppContext>::create(clp, env, base, clp->isHeadless());
	}

	ZooServer server(nullptr);
	server.configure(ctx);
	server.start(port);

	return app.exec();
}
