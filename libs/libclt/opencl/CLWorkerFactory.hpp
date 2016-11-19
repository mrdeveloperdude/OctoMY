#ifndef CLWORKERFACTORY_HPP
#define CLWORKERFACTORY_HPP

class CLWorker;
class CLThreadManager;

class CLWorkerFactory
{
public:
	explicit CLWorkerFactory();

public:
	virtual CLWorker * createInstance(CLThreadManager &man, int index)=0;
};

#endif // CLWORKERFACTORY_HPP
