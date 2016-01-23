#ifndef REMOTEMAIN_HPP
#define REMOTEMAIN_HPP

#include <QObject>
#include <QApplication>

#include "basic/StyleManager.hpp"

class RemoteMain: public QObject{
		Q_OBJECT
	private:
		StyleManager *sm;
		int ret;
		int argc;
		char **argv;
		QCoreApplication *app;
	public:
		explicit RemoteMain(int argc, char *argv[]);
		virtual ~RemoteMain();

	private slots:
		void run();

};

int main(int argc, char *argv[]);


#endif // REMOTEMAIN_HPP
