#ifdef USE_OC_OCL_STUFF

#ifndef CLWORKER_HPP
#define CLWORKER_HPP

#include "glt/IncludeOpenGL.hpp"
#include "clt/CLInclude.hpp"
#include "glt/GLContext.hpp"
#include "CLTool.hpp"

#include <QObject>
#include <QDebug>

class QThread;
class CLThreadManager;


/*!
 * \brief The CLWorker class is a companion class to CLThreadManager that is
 * meant to be inherited from by implementations that wish to carry out work in
 * OpnCL devices. Each instance will run on one device, and will be managed by
 * the CLThereadManager.
 *
 * The CLWorker interface has 4 phases:
 * 1. Constructor: This is where the managing class and index is passed to the worker.
 *    The manager class is responsible for starting/stopping/moving the worker
 *    to the right thread and the index is a unique ID for this worker
 * 2. preProcess: This is called just before processing starts. It is called when
 *    the worker is in the main/calling thread and is the last chance to do
 *    initializations that needs to be done in this thread.
 * 3. process: This is the main method where the bulk of the work is carried out.
 *    This function is called when the worker is in it's proper thread.
 *    It is expected that the worker carry out all OpenCL related processing here
 *    and loop until running is set to false, -or- until processing is complete
 *    in which case it is expected that mRunning is set to false.
 * 4. postProcess: This is called just after processing is done. It is called when
 *    the worker is back in the main/calling thread again and it is a chance to
 *    perform deinitializations that need to be carried out from this thread.
 *
 * Depending on the worker, it may or may not reset to be reused without having to
 * re-allocate it.
 *
 */
class CLWorker : public QObject, protected OCTOMY_QT_OGL_FUNCTIONS_CLASS
{
	Q_OBJECT

private:
	CLThreadManager &mManager;
	int mIndex;
	const cl::Device *mDev;
	bool mRunning;
	QThread *mThread;
	GLuint mPbo;
	cl::BufferGL *mPboBuff;
	cl::Context *mCtx;
	bool mIsInited;
	CLTool mCL;

public:
	explicit CLWorker(CLThreadManager &man, int index, QObject *parent = nullptr);
	virtual	~CLWorker();

public:
	void setRunning(const bool running);
	bool isRunning() const;
	bool isGLInteropWorker() const;
	QThread *thread() const;
	int index() const;
	GLuint pbo() const;
	cl::BufferGL *pboBuff() const;
	CLThreadManager &manager() const;
	cl::Context *clContext() const;
	const cl::Device *clDevice() const;
	GLContext *sharingGLContext() const;


protected:
	void setInitPBO(const bool init);
	void setInitCLContext(const bool init);
	void setInitCLContextOld(const bool init);

	void setInit(const bool init);

	// CLWorker interface wrappers
	//////////////////////////////////////////

public slots:
	void preProcess(QThread &th);
	void process();
	void postProcess();

	// CLWorker interface
	//////////////////////////////////////////

public:
	virtual void preProcessImp()=0;
	virtual void processImp()=0;
	virtual void postProcessImp()=0;

signals:
	void processFinished();

};

#endif
// CLWORKER_HPP


#endif
//USE_OC_OCL_STUFF
