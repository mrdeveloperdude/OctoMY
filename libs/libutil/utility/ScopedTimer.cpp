#include "ScopedTimer.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include<QDateTime>
#include <QDebug>
#include <QString>


ScopedTimer::ScopedTimer(QString name, bool doLog, bool, QString fmt)
	: startTime(QDateTime::currentMSecsSinceEpoch())
	, now(startTime)
	, interval(0)
	, doLog(doLog)
	//, human(human)
	, name(name)
	, fmt(fmt)
	, timeout(-1)
{
	OC_METHODGATE();
	/*
	if(doLog){
		qDebug() << getIntervalCompleteString(SCOPED_TIMER_SYMBOL " START   : ",false);
	}
	*/
}



ScopedTimer::ScopedTimer(QString name, qint64 timeout, bool ,QString fmt)
	: startTime(QDateTime::currentMSecsSinceEpoch())
	, now(startTime)
	, interval(0)
	, doLog(false)
	//, human(human)
	, name(name)
	, fmt(fmt)
	, timeout(timeout)
{
	OC_METHODGATE();
}

ScopedTimer::~ScopedTimer(){
	OC_METHODGATE();
	update();
	if(interval<=0){
		interval=1;
	}
	if(timeout>0 && interval>timeout){
		qWarning() << "WARNING: " << name<< " TIMED OUT WITH " << utility::humanReadableElapsedSeconds(((long double)interval)/1000.0f,true,true,0.001) << " > " << utility::humanReadableElapsedSeconds(((long double)timeout)/1000.0f,true,true,0.001);
	}
	else if(doLog){
		qDebug() << getIntervalCompleteString(SCOPED_TIMER_SYMBOL " END     : ",false);
	}
}

void ScopedTimer::update(){
	OC_METHODGATE();
	now=QDateTime::currentMSecsSinceEpoch();
	interval=now-startTime;
}

qint64 ScopedTimer::getInterval(bool up){
	OC_METHODGATE();
	if(up){
		update();
	}
	if(doLog){
		qDebug() << getIntervalCompleteString(SCOPED_TIMER_SYMBOL " INTERVAL: ",false);
	}
	return interval;
}

QString ScopedTimer::getIntervalCompleteString(QString prefix, bool up){
	OC_METHODGATE();
	if(up){
		update();
	}
	QString s="";
	s+=prefix;
	s+=name;
	s+=" - ";
	s+=getTimeFormated(fmt,false);
	if(interval>0){
		s+=" - ";
		s+=getIntervalHuman(2,true, false);
		s+=" (";
		s+=QString::number(interval);
		s+=" ms) ";
	}
	return s;
}

QString ScopedTimer::getIntervalHuman(int prec,bool prep, bool up){
	OC_METHODGATE();
	if(up){
		update();
	}
	QString ret=utility::humanReadableElapsedSeconds(((long double)interval)/1000.0f,prec,prep,0.001);
	return ret;
}

QString ScopedTimer::getTimeFormated(QString fmt, bool up){
	OC_METHODGATE();
	if(up){
		update();
	}
	QString ret=QDateTime::currentDateTime().toString(fmt) ;
	return ret;
}



