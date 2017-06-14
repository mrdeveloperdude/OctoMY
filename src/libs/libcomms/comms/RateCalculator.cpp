#include "RateCalculator.hpp"

#include "utility/Utility.hpp"

RateCalculator::RateCalculator(QString name)
	: mName(name)
	, mLast(0)
	, mCount(0)
	, mBytes(0)
	, mLastLog(0)
	, mCountLog(0)
	, mBytesLog(0)
{

}

void RateCalculator::countPacket(quint32 bytes)
{
	mLast=QDateTime::currentMSecsSinceEpoch();
	mCount++;
	mCountLog++;
	mBytes+=bytes;
	mBytesLog+=bytes;
	auto timeSinceLast=mLast-mLastLog;
	if(timeSinceLast>1000) {
		qreal packetRate=mCountLog;
		packetRate/=(qreal)timeSinceLast;
		packetRate/=(qreal)1000.0;
		qreal byteRate=mBytesLog;
		byteRate/=(qreal)timeSinceLast;
		byteRate/=(qreal)1000.0;
		qDebug().noquote() << mName << "Rate " << QString::number(packetRate) << " packets/sec, " << utility::humanReadableSize(packetRate) << "/sec, ";
		mLastLog=mLast;
		mCountLog=0;
		mBytesLog=0;
	}
}
