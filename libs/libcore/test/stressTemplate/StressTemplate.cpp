#include "StressTemplate.hpp"

void StressTemplate::stress(){
}


int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	//StressNoDave snd;
	qDebug()<<"Starting stress";
//	snd.stress();
	qDebug()<<"Stopping stress";
	app.exec();
	return EXIT_SUCCESS;
}

