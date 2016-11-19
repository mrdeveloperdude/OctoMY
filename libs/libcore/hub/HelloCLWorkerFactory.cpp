#include "HelloCLWorkerFactory.hpp"

#include "HelloCLWorker.hpp"

HelloCLWorkerFactory::HelloCLWorkerFactory()
{

}


CLWorker * HelloCLWorkerFactory::createInstance(CLThreadManager &man, int index)
{
	return new HelloCLWorker(man, index);
}
