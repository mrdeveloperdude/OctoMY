#ifndef CLWORKERFACTORY_HPP
#define CLWORKERFACTORY_HPP

class CLWorker;

class CLWorkerFactory
{
public:
	CLWorkerFactory();

public:
	virtual CLWorker * createInstance()=0;
};

#endif // CLWORKERFACTORY_HPP
