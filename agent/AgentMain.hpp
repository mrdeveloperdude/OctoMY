#ifndef AGENTMAIN_HPP
#define AGENTMAIN_HPP


#include <QObject>
#include <QApplication>

#include "basic/StyleManager.hpp"

class AgentMain: public QObject{
		Q_OBJECT
	private:
		StyleManager *sm;
		int ret;
		int argc;
		char **argv;
		QCoreApplication *app;
	public:
		explicit AgentMain(int argc, char *argv[]);
		virtual ~AgentMain();

	private slots:
		void run();

};

int main(int argc, char *argv[]);



#endif // AGENTMAIN_HPP
