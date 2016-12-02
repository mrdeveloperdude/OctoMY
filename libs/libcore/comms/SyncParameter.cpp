#include "SyncParameter.hpp"

#include <QDateTime>


SyncContext::SyncContext(quint64 rtt, quint64 now)
	: mNow(0)
	, mRTT(rtt)
{
	updateNow(now);
}

void SyncContext::updateNow(quint64 ts)
{
	if(0==ts) {
		ts=QDateTime::currentMSecsSinceEpoch();
	}
	mNow=ts;
}

quint64 SyncContext::roundTripTime() const
{
	return mRTT;
}

quint64 SyncContext::now() const
{
	return mNow;
}


