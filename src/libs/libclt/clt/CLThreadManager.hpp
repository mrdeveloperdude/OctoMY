#ifdef OC_USE_LIB_EXT_OPENCL

#ifndef CLTHREADMANAGER_HPP
#define CLTHREADMANAGER_HPP

#include "CLDeviceSelection.hpp"

#include "CLGLInteropConfig.hpp"

#include "glt/GLContext.hpp"

#include <QObject>
#include <QVector>
#include <QString>

class QThread;
class CLWorker;
class CLWorkerFactory;

/*!
 * \brief The CLThreadManager class is designed to start one thread per OpenCL
 * device matching the query passed as arguements to the constructor.
 *
 * Worker classes are created one per thread to carry out the OpenCL processing
 * while their running status and lifecycle is maintained.
 */
class CLThreadManager : public QObject
{
	Q_OBJECT

private:
	CLWorkerFactory &mFactory;
	CLDeviceSelection mDeviceSelection;
	CLGLInteropConfig mInteropConfig;
	QVector<CLWorker *> mWorkers;
	QVector<QThread *> mThreads;
	QThread *mDesiredCurrent;

public:
	explicit CLThreadManager(CLWorkerFactory &factory, CLGLInteropConfig config=CLGLInteropConfig(), QString deviceSelectionString="", bool allowGPU=true, bool allowCPU=false, QObject *parent = nullptr);
	virtual ~CLThreadManager();

public:
	// Returns true if we have an OpenCL device
	bool haveDevice();

	// Return wether or not this worker manager is in a running state
	bool isRunning()  const;
	// Set this worker manager to a running or stopped state.
	// When setting to a stopped state, you may specify block to have this call
	// wait for completion before returning
	void setRunning(bool running, bool block=false);
	// Return the CL device designated for the worker by index.
	// Will return nullptr if the index is out of range
	const cl::Device *device(int index) const;
	// Return the designated rendering worker when doing Gl-CL interop
	// Will return nullptr wheninterop is not enabled, or when no workers are available
	CLWorker *renderWorker();

	// Return the interop configuration currently in effect
	CLGLInteropConfig interopConfig() const;
};

#endif
// CLTHREADMANAGER_HPP

#endif
//OC_USE_LIB_EXT_OPENCL
