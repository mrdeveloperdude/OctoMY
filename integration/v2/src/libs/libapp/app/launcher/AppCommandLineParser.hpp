#ifndef APPCOMMANDLINEPARSER_HPP
#define APPCOMMANDLINEPARSER_HPP


#include <QCommandLineParser>

class AppCommandLineParser
{
private:
	QCommandLineParser mCommandlineOptions;

public:
	QCommandLineOption localHostOption;
	QCommandLineOption localPortOption;
	QCommandLineOption remoteHostOption;
	QCommandLineOption remotePortOption;
	QCommandLineOption headlessOption;

public:
	AppCommandLineParser();
public:

	// Process the actual command line arguments given by the user
	void process(int argc, char **argv);


	bool isHeadless();
};

#endif
// APPCOMMANDLINEPARSER_HPP
