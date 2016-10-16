#include "CLThreadManager.hpp"

#include "../libutil/utility/Standard.hpp"
#include "CLWorker.hpp"
#include "CLWorkerFactory.hpp"

#include <QThread>


CLThreadManager::CLThreadManager(CLWorkerFactory &factory, QString deviceSelectionString, bool allowGPU, bool allowCPU, QObject *parent)
	: QObject(parent)
	, mFactory(factory)
	, mDeviceSelection(deviceSelectionString, allowGPU, allowCPU)
{
	const size_t sz=mDeviceSelection.size();
	mWorkers.reserve(sz);
	mThreads.reserve(sz);
	for (size_t i = 0; i < sz; ++i) {
		mWorkers<<mFactory.createInstance();
		mThreads<<new QThread(this);
	}
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

bool CLThreadManager::isRunning()
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
				QThread *currentThread=QThread::currentThread();
				worker->preProcess(*thread);
				//From https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
				worker->moveToThread(thread);
				connect(thread, &QThread::started, worker, &CLWorker::process, OC_CONTYPE);
				connect(thread, &QThread::finished, this, [=]() {
					worker->moveToThread(currentThread);
					worker->postProcess();
				}, OC_CONTYPE);
				connect(worker, &CLWorker::processFinished, thread, &QThread::quit, OC_CONTYPE);
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
