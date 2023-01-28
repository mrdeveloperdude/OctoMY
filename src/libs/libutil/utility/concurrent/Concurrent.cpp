#include "Concurrent.hpp"

#include <QThread>
#include <QString>


namespace utility
{

namespace concurrent
{


HandleCounter::HandleCounter()
	: handleCounter(0)
	, handleMap()
	//, handleMutex
{

}


QString HandleCounter::handleCounterString(Qt::HANDLE h)
{
	if(nullptr==h) {
		return "H-null";
	}
	QMutexLocker lock(&handleMutex);
	if(!handleMap.contains(h)) {
		handleCounter++;
		handleMap.insert(h,handleCounter);
	}
	return "H-"+QString::number(handleMap[h]);
}


static HandleCounter hc;

QString currentThreadID()
{
	return hc.handleCounterString(QThread::currentThreadId());
}


}
}
