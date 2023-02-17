#ifdef OC_USE_LIB_EXT_OPENCL

#include "HelloCLWorker.hpp"

#include "clt/CLThreadManager.hpp"

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

#endif 
//OC_USE_LIB_EXT_OPENCL
