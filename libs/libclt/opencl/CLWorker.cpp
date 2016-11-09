#include "CLWorker.hpp"

CLWorker::CLWorker(QObject *parent)
	: QObject(parent)
{

}


void CLWorker::setRunning(bool running)
{
	mRunning=running;
}

bool CLWorker::isRunning() const
{
	return mRunning;
}




void CLWorker::preProcess(QThread &th)
{
	qWarning()<<"preProcess not implemented";
}

void CLWorker::process()
{
	qWarning()<<"process not implemented";
}

void CLWorker::postProcess()
{
	qWarning()<<"postProcess not implemented";
}
