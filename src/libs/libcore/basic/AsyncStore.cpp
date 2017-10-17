/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "AsyncStore.hpp"
#include "GenerateRunnable.hpp"

#include "utility/Standard.hpp"

#include <QThreadPool>
#include <QDebug>

AsyncStore::AsyncStore(QString filename, QObject *parent)
	: QObject(parent)
	, mReady(false)
	, mError(false)
	, mInProgress(false)
	, mFilename(filename)
{
	OC_METHODGATE();
	setObjectName("AsyncStore");
	//qDebug()<<"AsyncStore() file="<<mFilename;
}

void AsyncStore::bootstrapWorker()
{
	OC_METHODGATE();
	//qDebug()<<"AsyncStore bootstrapWorker() inProgress=" << mInProgress<<", error=" << mError<<", ready=" << mReady;
	if(!mInProgress) {
		mInProgress=true;
		bootstrapWorkerImpl();
		mInProgress=false;
	} else {
		qWarning()<<"ERROR: trying to call bootstrap while already in progress, skipping";
	}
}


void AsyncStore::bootstrap(bool loadOnly, bool runInBackground)
{
	OC_METHODGATE();
	//qDebug()<<"AsyncStore bootstrap() loadOnly="<<loadOnly<<", bg="<<runInBackground;
	if(mReady) {
		emit storeReady(!mError);
		return;
	} else if(mInProgress) {
		return;
	} else if(loadOnly) {
		load();
		return;
	}
	// QThreadPool takes ownership and deletes runnable automatically after completion
	else if(runInBackground) {
		QThreadPool *tp=QThreadPool::globalInstance();
		if(nullptr!=tp) {
			const bool ret=tp->tryStart(OC_NEW GenerateRunnable<AsyncStore>(*this));
			if(ret) {
				//qDebug()<<"ASYNCSTORE: Successfully started background thread";
				return;
			} else {
				tp->deleteLater();
				qWarning()<<"ERROR: Could not start runnable";
				return;
			}
		} else {
			qWarning()<<"ERROR: No global threadpool available, defaulting to serial version";
			return;
		}
		qWarning()<<"ERROR: Should not be here, falling back to serial bootstrap";
		return;
	}
	// Use single threaded way
	bootstrapWorker();
}


bool AsyncStore::isReady() // const not possible beacuse of atomics
{
	OC_METHODGATE();
	return mReady.get();
}

bool AsyncStore::isInProgress() // const not possible beacuse of atomics
{
	OC_METHODGATE();
	return mInProgress;
}

bool AsyncStore::hasError() // const not possible beacuse of atomics
{
	OC_METHODGATE();
	return mError;
}

bool AsyncStore::fileExists() const
{
	OC_METHODGATE();
	return QFile(mFilename).exists();
}


QString AsyncStore::filename() const
{
	OC_METHODGATE();
	return mFilename;
}

void AsyncStore::setHookAsyncStoreSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if(hook) {
		if(!connect(this,SIGNAL(storeReady()),&ob,SLOT(onStoreReady()),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(this,SIGNAL(storeReady()),&ob,SLOT(onStoreReady()))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}




