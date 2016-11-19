#ifndef HELLOCLWORKER_HPP
#define HELLOCLWORKER_HPP

#include "../libclt/opencl/CLWorker.hpp"


class CLThreadManager;


class HelloCLWorker : public CLWorker
{
private:

public:
	HelloCLWorker(CLThreadManager &man, int index, QObject *parent = nullptr);

	// CLWorker interface
	//////////////////////////////////////////
public slots:
	virtual void preProcessImp();
	virtual void processImp();
	virtual void postProcessImp();
};

#endif // HELLOCLWORKER_HPP
