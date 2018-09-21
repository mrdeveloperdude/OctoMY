#include "RateCalculator.hpp"

#include "utility/Utility.hpp"

RateCalculator::RateCalculator(QString name, const quint64 logInterval)
	: mName(name)
	, mLast(0)
	, mCount(0)
	, mBytes(0)
	, mLastLog(0)
	, mCountLog(0)
	, mBytesLog(0)
	, mLogInterval(logInterval)
{
	OC_METHODGATE();

}

void RateCalculator::countPacket(quint32 bytes, quint64 now)
{
	OC_METHODGATE();
	if(0==now) {
		now=utility::currentMsecsSinceEpoch<quint64>();
	}
	mLast=now;
	mCount++;
	mCountLog++;
	mBytes+=bytes;
	mBytesLog+=bytes;
	auto timeSinceLast=mLast-mLastLog;
	if( (mLogInterval > 0) && (timeSinceLast > mLogInterval) ) {
		qreal packetRate=mCountLog;
		packetRate /= static_cast<qreal>(timeSinceLast);
		packetRate /= static_cast<qreal>(mLogInterval);
		qreal byteRate=mBytesLog;
		byteRate /= static_cast<qreal>(timeSinceLast);
		byteRate /= static_cast<qreal>(mLogInterval);
		qDebug().noquote() << mName << "Rate " << QString::number(packetRate) << " packets/sec, " << utility::humanReadableSize(packetRate) << "/sec, ";
		mLastLog=mLast;
		mCountLog=0;
		mBytesLog=0;
	}
}




const QDebug &operator<<(QDebug &d, const RateCalculator &rc)
{
	OC_FUNCTIONGATE();
	d.noquote().nospace() << "RateCalculator("<<rc.mName<<"){mLast="<<rc.mLast <<", mCount="<<rc.mCount <<", mBytes="<<rc.mBytes <<", mLastLog="<<rc.mLastLog <<", mCountLog="<<rc.mCountLog <<", mBytesLog="<<rc.mBytesLog <<" , mLogInterval="<<rc.mLogInterval <<" }";
	return d.maybeSpace();
	//return d;
}




QString RateCalculator::operator()(RateCalculator &rc)
{
	QString out("RateCalculator(");
	out+=rc.mName;
	out+="){mLast=";
	out+=QString::number(rc.mLast);
	out+=", mCount=";
	out+=QString::number(rc.mCount);
	out+=", mBytes=";
	out+=QString::number(rc.mBytes);
	out+=", mLastLog=";
	out+=QString::number(rc.mLastLog);
	out+=", mCountLog=";
	out+=QString::number(rc.mCountLog);
	out+=", mBytesLog=";
	out+=QString::number(rc.mBytesLog);
	out+=" , mLogInterval=";
	out+=QString::number(rc.mLogInterval);
	out+=" }";
	return out;
}
