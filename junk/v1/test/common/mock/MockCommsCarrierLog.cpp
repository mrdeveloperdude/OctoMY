#include "MockCommsCarrierLog.hpp"

#include <QDebug>

MockCommsCarrierLog::MockCommsCarrierLog(QObject *parent)
	: QObject(parent)
	, mLastPrint(0)
	, mOpportunityCount(0)
{

}

MockCommsCarrierLog::~MockCommsCarrierLog()
{

}

void MockCommsCarrierLog::onCarrierReadyRead()
{
	qDebug()<<"MOCA::onCarrierReadyRead()";
}

void MockCommsCarrierLog::onCarrierError(const QString err)
{
	qDebug()<<"MOCA::onCarrierError(err="<<err<<")";
}

void MockCommsCarrierLog::onCarrierSendingOpportunity(const quint64 now)
{
	if(now - mLastPrint > 1000) {
		qDebug()<<"MOCA::onCarrierSendingOpportunity(opportunities=" << mOpportunityCount << " op/sec, now=" << now << ")";
		mOpportunityCount = 0;
		mLastPrint=now;
	}
	mOpportunityCount++;
}

void MockCommsCarrierLog::onCarrierConnectionStatusChanged(const bool connected)
{
	qDebug()<<"MOCA::onCarrierConnectionStatusChanged(connected="<<connected<<")";
}
