#ifndef APPCONTEXT_HPP
#define APPCONTEXT_HPP

#include "app/Settings.hpp"

#include "uptime/SharedPointerWrapper.hpp"

#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QString>


class AppCommandLineParser;

class AppContext
{

private:
	QSharedPointer<AppCommandLineParser> mCommandlineOptions;
	QProcessEnvironment mEnvironment;
	QSharedPointer<Settings> mSettings;
	QString mBase;
	QString mBaseDir;
	bool mIsHeadless;

public:
	explicit AppContext(QSharedPointer<AppCommandLineParser> commandlineOptions, QProcessEnvironment env, QString base, bool headless/*, QObject *parent=nullptr*/);
	virtual ~AppContext();

public:
	const QString base() const;
	const QString baseDir() const;
	QSharedPointer<Settings> settings();
	const QSharedPointer<AppCommandLineParser> commandLine() const;
	const QProcessEnvironment environment() const;
	bool isHeadless() const;

};

#endif
// APPCONTEXT_HPP
