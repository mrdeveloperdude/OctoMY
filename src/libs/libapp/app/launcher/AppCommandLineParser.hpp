#ifndef APPCOMMANDLINEPARSER_HPP
#define APPCOMMANDLINEPARSER_HPP

#include <QCommandLineParser>

class AppCommandLineParser
{
private:
	QCommandLineParser mCommandlineOptions;
	int mArgc;
	char **mArgv;

	bool mParseDone;
	bool mParseResult;

public:
	QCommandLineOption localHostOption;
	QCommandLineOption localPortOption;
	QCommandLineOption remoteHostOption;
	QCommandLineOption remotePortOption;
	QCommandLineOption headlessOption;

public:
	explicit AppCommandLineParser();
	virtual ~AppCommandLineParser();

public:
	// Process the actual command line arguments given by the user.
	void process(int argc, char **argv);

	// Has the parsing of commandline parameters completed (i.e. was process() called)?
	bool isParseDone();
	// Was the parsing of commandline parameters successful (i.e. no parsing errors)?
	bool isParseOK();

	// Did the parameters say we are headless or not? Will return false if parameters were not parsed yet.
	bool isHeadless();

	// Provide original argument count
	int argc();

	// Provide original argument array
	char **argv();

};

#endif
// APPCOMMANDLINEPARSER_HPP
