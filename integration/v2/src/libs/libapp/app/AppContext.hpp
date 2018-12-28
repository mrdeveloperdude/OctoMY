#ifndef APPCONTEXT_HPP
#define APPCONTEXT_HPP

#include "app/Settings.hpp"

#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QString>


class AppContext: public QObject
{
		Q_OBJECT
	private:

		QCommandLineParser &mCommandlineOptions;
		QProcessEnvironment &mEnvironment;
		Settings mSettings;
		QString mBase;
		QString mBaseDir;

	public:
		explicit AppContext(QCommandLineParser &opts, QProcessEnvironment &env, QString base, QObject *parent=nullptr);

	public:

		inline const QString base() const
		{
			return mBase;
		}


		inline const QString baseDir() const
		{
			return mBaseDir;
		}

		inline Settings &settings()
		{
			return mSettings;
		}

		inline const QCommandLineParser &commandLine() const
		{
			return mCommandlineOptions;
		}


		inline const QProcessEnvironment &environment() const
		{
			return mEnvironment;
		}

};

#endif // APPCONTEXT_HPP
