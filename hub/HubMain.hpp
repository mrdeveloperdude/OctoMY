#ifndef SERVERMAIN_HPP
#define SERVERMAIN_HPP

#include <QObject>
#include <QApplication>

#include "../libs/libcore/basic/StyleManager.hpp"

class HubMain:public QObject{
		Q_OBJECT
	private:
		StyleManager *sm;
		int ret;
		int argc;
		char **argv;
		QCoreApplication *app;
	public:
		explicit HubMain(int argc, char *argv[]);
		virtual ~HubMain();

	private slots:
		void run();

};

int main(int argc, char *argv[]);


#endif // SERVERMAIN_HPP
