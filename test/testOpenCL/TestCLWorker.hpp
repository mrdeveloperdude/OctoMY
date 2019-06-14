#ifndef TESTCLWORKER_HPP
#define TESTCLWORKER_HPP

#include "Common_test.hpp"

#include "clt/CLWorker.hpp"



class TestCLWorker: public CLWorker
{
private:
	cl::CommandQueue queue;
	cl::Program::Sources source;
	cl::Program program;
	cl::Kernel kernel;
	QVector<QPoint> sources;
	QMutex sourceChangeMutex;
	float limit;
	bool applyLimit;
	float charge;
	QString kernelParams;

public:
	explicit TestCLWorker(CLThreadManager &man, int index);
	virtual	~TestCLWorker() Q_DECL_OVERRIDE;

public:
	void initCLStuff();
	void loadCLProgram();
	void preparePoints();
	void calculateTexture();
	void changeMousePoint(QPoint point);

	// CLWorker interface
public:
	void preProcessImp() Q_DECL_OVERRIDE;
	void processImp() Q_DECL_OVERRIDE;
	void postProcessImp() Q_DECL_OVERRIDE;

};



#endif
// TESTCLWORKER_HPP
