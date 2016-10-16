#include "HelloCLWorker.hpp"

#include <QThread>


HelloCLWorker::HelloCLWorker()
{

}




void HelloCLWorker::preProcess(QThread &th)
{
	qWarning()<<"HELLO! preProcessing";
}

void HelloCLWorker::process()
{
	qWarning()<<"HELLO! processing started";
	while(isRunning()) {
		qWarning()<<"HELLO! processing...";
		QThread::msleep(1000);
	}
	qWarning()<<"HELLO! processing ended";
	emit processFinished();
}

void HelloCLWorker::postProcess()
{
	qWarning()<<"HELLO! postProcessing";
}
