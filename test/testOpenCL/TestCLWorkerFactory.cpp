#include "TestCLWorkerFactory.hpp"

#include "Common_test.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "TestCLWorker.hpp"


CLWorker * TestCLWorkerFactory::createInstance(CLThreadManager &man, int index)
{
	OC_METHODGATE();
	qDebug()<<"TCLWF CREATED WORKER WITH INDEX "<<index;
	TestCLWorker *tw=OC_NEW TestCLWorker(man, index);
	return tw;
}

TestCLWorkerFactory::~TestCLWorkerFactory()
{
	OC_METHODGATE();

}
