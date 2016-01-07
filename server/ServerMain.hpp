#ifndef SERVERMAIN_HPP
#define SERVERMAIN_HPP

#include <QObject>
#include <QApplication>

#include "basic/StyleManager.hpp"

class ServerMain:public QObject{
		Q_OBJECT
	private:
		StyleManager *sm;
		int ret;
		int argc;
		char **argv;
		QCoreApplication *app;
	private:
		QCoreApplication*createApplication(int &argc, char *argv[], bool useGUI);
	public:
		explicit ServerMain(int argc, char *argv[]);
		virtual ~ServerMain();

	private slots:
		void run();

};

int main(int argc, char *argv[]);


#endif // SERVERMAIN_HPP
