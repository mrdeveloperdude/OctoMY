#include "HelloCLWorker.hpp"

#include "../libclt/opencl/CLThreadManager.hpp"

#include <QThread>


HelloCLWorker::HelloCLWorker(CLThreadManager &man, int index, QObject *parent)
	: CLWorker(man, index, parent)
{
	qDebug()<<"HELLO! CTOR started in thread " <<QThread::currentThreadId();
}




void HelloCLWorker::preProcessImp()
{
	qWarning()<<"HELLO! pre...";
}

void HelloCLWorker::processImp()
{
	while(isRunning()) {
		qWarning()<<"HELLO! processing...";
		QThread::msleep(1000);
	}
}

void HelloCLWorker::postProcessImp()
{
	qWarning()<<"HELLO! post...";
}
