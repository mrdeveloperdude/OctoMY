#ifndef CLWORKERFACTORY_HPP
#define CLWORKERFACTORY_HPP

class CLWorker;
class CLThreadManager;
/*!
 * \brief The CLWorkerFactory class is the base class for generating decendents
 * of CLWorkers of a spesific type. It is used by CLThreadManager to isntanciate
 * the correct worker types for each thread.
  */
class CLWorkerFactory
{
public:
	explicit CLWorkerFactory();

public:
	virtual CLWorker * createInstance(CLThreadManager &man, int index)=0;
};

#endif // CLWORKERFACTORY_HPP
