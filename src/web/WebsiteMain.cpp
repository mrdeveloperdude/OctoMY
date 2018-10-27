#include "WebsiteMain.hpp"

#include "Website.hpp"

#include "utility/Utility.hpp"
#include "basic/LogHandler.hpp"
#include "node/AppContext.hpp"


#include <QCoreApplication>

#include <QCommandLineParser>
#include <QProcessEnvironment>


int main(int argc, char *argv[])
{
	qsrand(utility::currentMsecsSinceEpoch<quint32>());
	LogHandler::setLogging(true);

	//QCoreApplication app(argc, argv);

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

	Website website(OC_NEW AppContext(opts, env, "website", nullptr));
	website.run();
//	app.exit(0);
	//return app.exec();
}


