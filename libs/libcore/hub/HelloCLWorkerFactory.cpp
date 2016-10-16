#include "HelloCLWorkerFactory.hpp"

#include "HelloCLWorker.hpp"

HelloCLWorkerFactory::HelloCLWorkerFactory()
{

}


CLWorker * HelloCLWorkerFactory::createInstance()
{
	return new HelloCLWorker;
}
