#ifndef APPCONTEXT_HPP
#define APPCONTEXT_HPP

#include "Settings.hpp"

#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QString>


class AppContext: public QObject
{
		Q_OBJECT
	private:

		QCommandLineParser &mOpts;
		QProcessEnvironment &mEnv;
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

		inline const QCommandLineParser &options() const
		{
			return mOpts;
		}


		inline const QProcessEnvironment &environment() const
		{
			return mEnv;
		}

};

#endif // APPCONTEXT_HPP
