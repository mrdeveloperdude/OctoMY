#include "FPSCalculator.hpp"

#include <QDateTime>
#include <QDebug>


FPSCalculator::FPSCalculator(QString name, quint64 logInterval)
	: name(name)
	, logInterval(logInterval)
	, frameAccumulator(0)
	, lastTime(0)

{

}


FPSCalculator::~FPSCalculator()
{
	frameAccumulator=0;
	lastTime=0;
}

void FPSCalculator::update()
{
	quint64 now=QDateTime::currentMSecsSinceEpoch();
	quint64 interval=now-lastTime;
	frameAccumulator++;
	if(interval>logInterval) {
		double fps=frameAccumulator*1000;
		fps/=interval;
		qDebug().noquote().nospace()<<"%%%% '"<<name<<"' FPS="<<fps;
		lastTime=now;
		frameAccumulator=0;
	}
}
