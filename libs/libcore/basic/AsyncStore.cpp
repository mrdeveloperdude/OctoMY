/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "AsyncStore.hpp"


#include "GenerateRunnable.hpp"

#include <QThreadPool>
#include <QDebug>

AsyncStore::AsyncStore(QString filename, QObject *parent)
	: QObject(parent)
	, mReady(false)
	, mError(false)
	, mInProgress(false)
	, mFilename(filename)
{
	setObjectName("AsyncStore");
	//qDebug()<<"AsyncStore() file="<<mFilename;
}

void AsyncStore::bootstrapWorker()
{
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
			const bool ret=tp->tryStart(new GenerateRunnable<AsyncStore>(*this));
			if(ret) {
				//qDebug()<<"ASYNCSTORE: Successfully started background thread";
				return;
			} else {
				tp->deleteLater();
				qWarning()<<"ERROR: Could not start runnable";
			}
		} else {
			qWarning()<<"ERROR: No global threadpool available, defaulting to serial version";
		}
		//qDebug()<<"ASYNCSTORE: Falling back to serial bootstrap";
	}
	// Use single threaded way
	bootstrapWorker();
}


bool AsyncStore::isReady()
{
	return mReady.get();
}

bool AsyncStore::isInProgress()
{
	return mInProgress;
}

bool AsyncStore::hasError()
{
	return mError;
}

bool AsyncStore::fileExists() const
{
	return QFile(mFilename).exists();
}
