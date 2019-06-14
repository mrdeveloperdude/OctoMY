#ifndef TESTCLWORKERFACTORY_HPP
#define TESTCLWORKERFACTORY_HPP

#include "clt/CLWorkerFactory.hpp"

class CLWorker;
class CLThreadManager;


class TestCLWorkerFactory: public CLWorkerFactory
{
public:
	CLWorker * createInstance(CLThreadManager &man, int index);
	virtual ~TestCLWorkerFactory();
};


#endif
// TESTCLWORKERFACTORY_HPP
