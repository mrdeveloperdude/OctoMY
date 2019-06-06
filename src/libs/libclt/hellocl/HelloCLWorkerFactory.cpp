#include "HelloCLWorkerFactory.hpp"

#include "utility/Standard.hpp"

#include "HelloCLWorker.hpp"

HelloCLWorkerFactory::HelloCLWorkerFactory()
{

}


CLWorker * HelloCLWorkerFactory::createInstance(CLThreadManager &man, int index)
{
	return OC_NEW HelloCLWorker(man, index);
}
