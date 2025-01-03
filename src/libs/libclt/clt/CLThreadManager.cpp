#ifdef OC_USE_LIB_EXT_OPENCL

#include "CLThreadManager.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "CLWorker.hpp"
#include "CLWorkerFactory.hpp"

#include <QThread>


CLThreadManager::CLThreadManager(CLWorkerFactory &factory, CLGLInteropConfig config, QString deviceSelectionString, bool allowGPU, bool allowCPU, QObject *parent)
	: QObject(parent)
	, mFactory(factory)
	, mDeviceSelection(deviceSelectionString, allowGPU, allowCPU)
	, mInteropConfig(config)
	, mDesiredCurrent(QThread::currentThread())
{
	OC_METHODGATE();
	if(haveDevice()) {
		const auto sz=mDeviceSelection.size();
		mWorkers.reserve(sz);
		mThreads.reserve(sz);
		for (int i = 0; i < sz; ++i) {
			CLWorker *worker=mFactory.createInstance(*this, i);
			QThread *thread=OC_NEW QThread(this);
			if(nullptr==thread) {
				qWarning()<<"ERROR: Thread was null";
				continue;
			}
			if(nullptr==worker) {
				qWarning()<<"ERROR: Worker was null";
				continue;
			}
			mWorkers<<worker;
			mThreads<<thread;
			connect(thread, &QThread::started, worker, &CLWorker::process, OC_CONTYPE);
			connect(thread, &QThread::finished, this, [=]() {
				QThread *currentThread2=QThread::currentThread();
				qDebug()<<"CURTHREAD2: "<<currentThread2->currentThreadId();
				QThread *wthread2=worker->thread();
				qDebug()<<"WTHREAD2: "<<wthread2->currentThreadId();
				if(wthread2->currentThreadId()!=currentThread2->currentThreadId()) {
					worker->moveToThread(currentThread2);
					QThread *wthread3=worker->thread();
					qDebug()<<"WTHREAD3: "<<wthread3->currentThreadId();
					if(wthread3->currentThreadId()!=currentThread2->currentThreadId()) {
						qWarning()<<"SECOND MOVE FAILED!";
					}

				} else {
					qDebug()<<"SKIPPING THREAD MOVE, ALREADY IN RIGHT THREAD";
				}
				worker->postProcess();
			}, OC_CONTYPE);
			connect(worker, &CLWorker::processFinished, thread, &QThread::quit, OC_CONTYPE);
		}
		qDebug()<<"CURTHREADDES: "<<mDesiredCurrent->currentThreadId();
	} else {
		qWarning()<<"ERROR: No suitable OpenCL devices found.";
	}
}


CLThreadManager::~CLThreadManager()
{
	OC_METHODGATE();
	setRunning(false,true);
	for(QThread *thread:mThreads) {
		delete thread;
		thread=nullptr;
	}
	mThreads.clear();
	for(CLWorker *worker:mWorkers) {
		delete worker;
		worker=nullptr;
	}
	mWorkers.clear();
}


bool CLThreadManager::haveDevice()
{
	OC_METHODGATE();
	return mDeviceSelection.size()>0;
}


bool CLThreadManager::isRunning() const
{
	OC_METHODGATE();
	const auto sz=mThreads.size();
	for (int i = 0; i < sz; ++i) {
		QThread *thread=mThreads[i];
		if(nullptr==thread) {
			qWarning()<<"ERROR: Thread was null";
			continue;
		}
		if(thread->isRunning()) {
			return true;
		}
	}
	return false;
}


void CLThreadManager::setRunning(bool running, bool block)
{
	OC_METHODGATE();
	if(haveDevice()) {
		const auto sz=mDeviceSelection.size();
		const bool isAlreadyuRunning=isRunning();
		qDebug()<<(running?"STARTING ":"STOPPING ")<<sz<<" CL Thread(s)------------------------";
		if(running && block) {
			qWarning()<<"PLEASE NOTE THAT BLOCK WILL NOT WORK FOR RUNNING, ONLY FOR STOPPING";
		}
		if(running!=isAlreadyuRunning) {
			QThread *currentThread=QThread::currentThread();
			qDebug()<<"CURTHREAD1: "<<currentThread->currentThreadId();
			if(currentThread->currentThreadId()!=mDesiredCurrent->currentThreadId()) {
				qWarning()<<"ERROR: setRunning not called from same thread as CTOR 1";
			}
			if(running) {
				for (int i = 0; i < sz; ++i) {
					CLWorker *worker=mWorkers[i];
					QThread *thread=mThreads[i];
					if(nullptr==thread) {
						qWarning()<<"ERROR: Thread was null";
						continue;
					}
					if(nullptr==worker) {
						qWarning()<<"ERROR: Worker was null";
						continue;
					}
					worker->preProcess(*thread);
					//From https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
					worker->moveToThread(thread);
					QThread *wthread1=worker->thread();
					qDebug()<<"WTHREAD1: "<<wthread1->currentThreadId();
					if(wthread1->currentThreadId()!=thread->currentThreadId()) {
						qWarning()<<"FIRST MOVE FAILED!";
					}

				}
				for (int i = 0; i < sz; ++i) {
					CLWorker *worker=mWorkers[i];
					QThread *thread=mThreads[i];
					if(nullptr==thread) {
						qWarning()<<"ERROR: Thread was null";
						continue;
					}
					if(nullptr==worker) {
						qWarning()<<"ERROR: Worker was null";
						continue;
					}
					worker->setRunning(true);
					thread->start();
				}

			} else {
				for (int i = 0; i < sz; ++i) {
					mWorkers[i]->setRunning(false);
				}
				if(block) {
					if(running) {
					} else {
						qDebug()<<" + Blocking while waiting for threads to complete...";
						for (int i = 0; i < sz; ++i) {
							QThread *thread=mThreads[i];
							if(nullptr==thread) {
								qWarning()<<"ERROR: Thread was null";
								continue;
							}
							qDebug()<<"JOIN start for thread "<<i;
							thread->wait();
							qDebug()<<"JOIN done for thread "<<i;
						}
					}
				}
			}
		} else {
			qDebug()<<" + Already "<<(running?"STARTED ":"STOPPED")<<", skipping...";
		}
	}
}


const cl::Device *CLThreadManager::device(int index) const
{
	OC_METHODGATE();
	if(index<0 || index>=mDeviceSelection.count()) {
		return nullptr;
	}
	return &mDeviceSelection.at(index);
}


CLWorker *CLThreadManager::renderWorker()
{
	OC_METHODGATE();
	//Trivial reject, we won't be rendering so no render worker
	if(!mInteropConfig.doGLInterop()) {
		return nullptr;
	}
	if(mWorkers.size()<=0) {
		return nullptr;
	}
	CLWorker *worker=mWorkers.at(0);
	if(nullptr==worker) {
		qWarning()<<"ERROR: worker was unexpected null";
	} else if(!worker->isGLInteropWorker()) {
		qWarning()<<"ERROR: worker was not render worker as expected";
		return nullptr;
	}
	return worker;
}


CLGLInteropConfig CLThreadManager::interopConfig()  const
{
	OC_METHODGATE();
	return mInteropConfig;
}


#endif
// OC_USE_LIB_EXT_OPENCL
