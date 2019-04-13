#include "ScopedTimer.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"
#include "utility/string/String.hpp"


#include <QDateTime>
#include <QDebug>
#include <QString>

//explicit ScopedTimer(QString mName = "", bool log = true, ScopedTimerFormat format = AUTO, QString mFormatString="yyyy-MM-dd hh:mm:ss.zzz");
//explicit ScopedTimer(QString mName, qint64 mTimeout, ScopedTimerFormat format = AUTO, QString mFormatString="yyyy-MM-dd hh:mm:ss.zzz");


ScopedTimer::ScopedTimer(QString name, bool doLog, ScopedTimerFormat format, QString formatString)
	: mStartTime(::utility::time::currentMsecsSinceEpoch<qint64>())
	, mNow(mStartTime)
	, mInterval(0)
	, mDoLog(doLog)
	, mName(name)
	, mFormatString(formatString)
	, mTimeout(-1)
	, mFormat(format)
{
	OC_METHODGATE();
	doAutoFormat();
	/*
	if(doLog){
		qDebug() << getIntervalCompleteString(SCOPED_TIMER_SYMBOL " START   : ",false);
	}
	*/
}



ScopedTimer::ScopedTimer(QString name, qint64 timeout, ScopedTimerFormat format, QString formatString)
	: mStartTime(utility::time::currentMsecsSinceEpoch<qint64>())
	, mNow(mStartTime)
	, mInterval(0)
	, mDoLog(false)
	, mName(name)
	, mFormatString(formatString)
	, mTimeout(timeout)
	, mFormat(format)
{
	OC_METHODGATE();
}

ScopedTimer::~ScopedTimer()
{
	OC_METHODGATE();
	doAutoFormat();
	update();
	if(mInterval<=0) {
		mInterval=1;
	}
	if(mTimeout>0 && mInterval>mTimeout) {
		qWarning().noquote().nospace() << "WARNING: " << mName<< " TIMED OUT WITH " << utility::string::humanReadableElapsedSeconds((static_cast<long double>(mInterval))/1000.0l, true, true, 0.001l) << " > " << utility::string::humanReadableElapsedSeconds((static_cast<long double>(mTimeout))/1000.0l, true, true, 0.001l);
	} else if(mDoLog) {
		qDebug().noquote().nospace() << getIntervalCompleteString( "     " SCOPED_TIMER_SYMBOL " END: ", false);
	}
}


void ScopedTimer::doAutoFormat()
{
	OC_METHODGATE();
	if(STF_AUTO == mFormat) {
		mFormat = STF_ELAPSED;
		if("" != mName) {
			mFormat = static_cast<ScopedTimerFormat>(STF_NAME | mFormat);
		}
		if("" != mFormatString) {
			mFormat = static_cast<ScopedTimerFormat>(STF_TIME | mFormat);
		}
	}
}

void ScopedTimer::update()
{
	OC_METHODGATE();
	mNow = utility::time::currentMsecsSinceEpoch<qint64>();
	mInterval = mNow - mStartTime;
}

qint64 ScopedTimer::getInterval(bool up)
{
	OC_METHODGATE();
	if(up) {
		update();
	}
	if(mDoLog) {
		qDebug() << getIntervalCompleteString(SCOPED_TIMER_SYMBOL " INTERVAL: ", false);
	}
	return mInterval;
}

QString ScopedTimer::getIntervalCompleteString(QString prefix, bool up)
{
	OC_METHODGATE();
	if(up) {
		update();
	}
	QString s="";
	s += prefix;
	if(mFormat & STF_NAME) {
		s += mName;
	}
	if(mFormat & STF_TIME) {
		s += " - ";
		s += getTimeFormated(mFormatString, false);
	}
	if(mInterval > 0) {
		if(mFormat & STF_HUMAN_ELAPSED) {
			s += " - ";
			s += getIntervalHuman(2, true, false);
		}
		if(mFormat & STF_ELAPSED) {
			s += " (";
		}
		if(mFormat & STF_MILLIS_ELAPSED) {
			s += QString::number(mInterval);
			s += " ms";
		}
		if(mFormat & STF_ELAPSED) {
			s += ") ";
		}
	}
	return s;
}

QString ScopedTimer::getIntervalHuman(int prec,bool prep, bool up)
{
	OC_METHODGATE();
	if(up) {
		update();
	}
	QString ret=utility::string::humanReadableElapsedSeconds((static_cast<long double>(mInterval))/1000.0l, prec, prep, 0.001l);
	return ret;
}

QString ScopedTimer::getTimeFormated(QString fmt, bool up)
{
	OC_METHODGATE();
	if(up) {
		update();
	}
	QString ret=QDateTime::currentDateTime().toString(fmt) ;
	return ret;
}
