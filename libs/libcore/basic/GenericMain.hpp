#ifndef GENERICMAIN_HPP
#define GENERICMAIN_HPP



#include <QObject>
#include <QApplication>
#include <QCommandLineParser>

#include "../libcore/basic/StyleManager.hpp"


class GenericMain: public QObject{
		Q_OBJECT
	private:
		StyleManager *sm;
		int ret;
		int argc;
		char **argv;
		QCoreApplication *app;
	protected:
		QCommandLineParser parser;
		bool headless;

	public:
		explicit GenericMain(int argc, char *argv[]);
		virtual ~GenericMain();

		void run();

		virtual void start()=0;
		virtual void stop()=0;

	private slots:
		void inLoop();
};

#endif // GENERICMAIN_HPP
