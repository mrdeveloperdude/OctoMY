#include "RemoteMain.hpp"

#include "basic/RemoteWindow.hpp"
#include "basic/Remote.hpp"
#include "comms/CommsChannel.hpp"

#include <QDebug>
#include <QTimer>

#include <QApplication>



RemoteMain::RemoteMain(int argc, char *argv[]):
	QObject(0)
  , sm(0)
  , ret(EXIT_SUCCESS)
  , argc(argc)
  , argv(argv)
  , app(0)
{
	app=createApplication(argc,argv,true);
	if(0!=app){
		Q_INIT_RESOURCE(style);
		Q_INIT_RESOURCE(fonts);
		Q_INIT_RESOURCE(icons);
		QTimer::singleShot(0, this, SLOT(run()));
		ret=app->exec();
	}
	else{
		qWarning()<<"ERROR: no app, quitting";
	}
}

RemoteMain::~RemoteMain(){}


void RemoteMain::run(){
	sm=new StyleManager;
	if(0!=sm){
		sm->apply();
	}
}

QCoreApplication* RemoteMain::createApplication(int &argc, char *argv[], bool useGUI){
	if(useGUI){
		qDebug()<<"GUI ENABLED";
		QCoreApplication *a=new QApplication(argc, argv);
		Remote *remote=new Remote;
		RemoteWindow *w=new RemoteWindow (remote,0);
		w->show();
		return a;
	}
	else{
		qDebug()<<"HEADLESS";
		return new QCoreApplication(argc, argv);
	}
}


int main(int argc, char *argv[]){
	RemoteMain m(argc,argv);
	//RemoteMain m;
}


