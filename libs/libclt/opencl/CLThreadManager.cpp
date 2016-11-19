#include "CLThreadManager.hpp"

#include "../libutil/utility/Standard.hpp"
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
	const size_t sz=mDeviceSelection.size();
	mWorkers.reserve(sz);
	mThreads.reserve(sz);
	for (size_t i = 0; i < sz; ++i) {
		CLWorker *worker=mFactory.createInstance(*this, i);
		QThread *thread=new QThread(this);
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
}


CLThreadManager::~CLThreadManager()
{
	setRunning(false,true);
	for(QThread *thread:mThreads) {
		delete thread;
	}
	mThreads.clear();
	for(CLWorker *worker:mWorkers) {
		delete worker;
	}
	mWorkers.clear();
}

bool CLThreadManager::isRunning() const
{
	const size_t sz=mThreads.size();
	for (size_t i = 0; i < sz; ++i) {
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
	const size_t sz=mDeviceSelection.size();
	const bool isAlreadyuRunning=isRunning();
	qDebug()<<(running?"STARTING ":"STOPPING ")<<sz<<" CL Thread(s)------------------------";
	if(running!=isAlreadyuRunning) {
		QThread *currentThread=QThread::currentThread();
		qDebug()<<"CURTHREAD1: "<<currentThread->currentThreadId();
		if(currentThread->currentThreadId()!=mDesiredCurrent->currentThreadId()) {
			qWarning()<<"ERROR: setRunning not called from same thread as CTOR 1";
		}
		if(running) {
			for (size_t i = 0; i < sz; ++i) {
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
			for (size_t i = 0; i < sz; ++i) {
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
			for (size_t i = 0; i < sz; ++i) {
				mWorkers[i]->setRunning(false);
			}
			if(block) {
				if(running) {
				} else {
					qDebug()<<" + Blocking while waiting for threads to complete...";
					for (size_t i = 0; i < sz; ++i) {
						QThread *thread=mThreads[i];
						if(nullptr==thread) {
							qWarning()<<"ERROR: Thread was null";
							continue;
						}
						qDebug()<<"JOIN start "<<i;
						thread->wait();
						qDebug()<<"JOIN done "<<i;
					}
				}
			}
		}
	} else {
		qDebug()<<" + Already "<<(running?"STARTED ":"STOPPED")<<", skipping...";
	}
}


const cl::Device *CLThreadManager::device(int index) const
{
	if(index<0 || index>=mDeviceSelection.count()) {
		return nullptr;
	}
	return &mDeviceSelection.at(index);
}


CLGLInteropConfig CLThreadManager::interopConfig()  const
{
	return mInteropConfig;
}
