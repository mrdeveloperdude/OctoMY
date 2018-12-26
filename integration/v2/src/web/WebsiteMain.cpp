#include "WebsiteMain.hpp"

#include "Website.hpp"

#include "utility/time/HumanTime.hpp"
#include "uptime/MethodGate.hpp"
#include "app/LogHandler.hpp"
//#include "basic/TopicGenerator.hpp"
#include "app/AppContext.hpp"

#include "GoogleSitesClassicTool.hpp"


#include <QCoreApplication>

#include <QCommandLineParser>
#include <QProcessEnvironment>


int main(int argc, char *argv[])
{
	OC_FUNCTIONGATE();
	qsrand(utility::time::currentMsecsSinceEpoch<quint32>());
	LogHandler::setLogging(true);

	QCoreApplication app(argc, argv), *papp=&app;

	//Q_INIT_RESOURCE(webpages);

	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();

	QCommandLineParser opts;

	opts.setApplicationDescription("Website generator for OctoMY™");
	opts.addHelpOption();
	opts.addVersionOption();

	QCommandLineOption outputFolderOption(QStringList() <<  "o" << "output-dir", QCoreApplication::translate("main", "Select the folder into which the website directory structure will be created. NOTE: The tool will create this folder if it does not exist. Defaults to current directory (pwd)"), QCoreApplication::translate("main", "output-dir"));
	opts.addOption(outputFolderOption);
	QCommandLineOption inputFolderOption(QStringList() <<  "i" << "input-dir", QCoreApplication::translate("main", "Select the folder from which the website structure and content will be read. NOTE: This folder must exist."), QCoreApplication::translate("main", "input-dir"));
	opts.addOption(inputFolderOption);

	// Process the actual command line arguments given by the user
	QStringList arguments;
	for(int i=0; i<argc; ++i) {
		arguments<<argv[i];
	}
	opts.process(arguments);

	GoogleSitesClassicTool gsct;
	gsct.scavenge();

	/*
	TopicGenerator tg;
	QString out;
	for(int i=0;i<1000;++i)out.append(tg.generate()+"\n");
	utility::stringToFile("/tmp/topics.txt", out);
	Website website(OC_NEW AppContext(opts, env, "website", nullptr));
	website.run();
	*/
	QTimer::singleShot(10000, [=]() {
		papp->exit(0);
	});

	return app.exec();
}


