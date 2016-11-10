#ifndef HELLOCLWORKER_HPP
#define HELLOCLWORKER_HPP

#include "../libclt/opencl/CLWorker.hpp"

class HelloCLWorker : public CLWorker
{
public:
	HelloCLWorker();

public slots:
	virtual void preProcess(QThread &th);
	virtual void process();
	virtual void postProcess();
};

#endif // HELLOCLWORKER_HPP
