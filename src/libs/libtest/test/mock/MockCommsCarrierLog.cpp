#include "MockCommsCarrierLog.hpp"

#include <QDebug>

MockCommsCarrierLog::MockCommsCarrierLog(QObject *parent, const QString &sender)
	: QObject(parent)
	, mLastPrint(0)
	, mOpportunityCount(0)
	, mSender(sender)
{

}


MockCommsCarrierLog::~MockCommsCarrierLog()
{

}


void MockCommsCarrierLog::onCarrierReadyRead()
{
	emit logMessage(mSender, "ready", "carrier-ready-read");
	qDebug()<<"MOCA::onCarrierReadyRead()";
}


void MockCommsCarrierLog::onCarrierError(const QString err)
{
	emit logMessage(mSender, err, "carrier-error");
	qDebug()<<"MOCA::onCarrierError(err="<<err<<")";
}


void MockCommsCarrierLog::onCarrierSendingOpportunity(const quint64 now)
{
	if(now - mLastPrint > 1000) {
		emit logMessage(mSender, QString("opportunities=%1 op/sec, now=%2").arg(mOpportunityCount).arg(now), "carrier-sending-opertunity");
		qDebug()<<"MOCA::onCarrierSendingOpportunity(opportunities=" << mOpportunityCount << " op/sec, now=" << now << ")";
		mOpportunityCount = 0;
		mLastPrint = now;
	}
	mOpportunityCount++;
}


void MockCommsCarrierLog::onCarrierConnectionStatusChanged(const bool connected)
{
	emit logMessage(mSender, QString("connected=%1").arg(connected), "carrier-connection-status");
	qDebug()<<"MOCA::onCarrierConnectionStatusChanged(connected="<<connected<<")";
}
