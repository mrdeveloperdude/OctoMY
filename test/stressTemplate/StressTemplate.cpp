#include "StressTemplate.hpp"


// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void StressTemplate::stress(){
}


int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	qDebug()<<"Starting stress";
	qDebug()<<"Stopping stress";
	app.exec();
	return EXIT_SUCCESS;
}

