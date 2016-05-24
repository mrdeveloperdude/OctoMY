#include "ZooMain.hpp"
#include "zoo/ZooServer.hpp"

#include <QCoreApplication>
#include <QProcessEnvironment>

int main(int argc, char *argv[]){
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

	ZooServer server;
	server.start(port);

	return app.exec();}
