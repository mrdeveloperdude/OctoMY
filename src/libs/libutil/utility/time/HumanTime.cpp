#include "utility/time/HumanTime.hpp"

#include <QCoreApplication>

#include <cmath>
#include <qtimezone.h>
//#include <cfloat>

namespace utility
{


namespace time
{


void delay( int millisecondsToWait )
{
	QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
	while( QTime::currentTime() < dieTime ) {
		QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
	}
}


quint64 variantToMs(QVariant v)
{
	return static_cast<quint64>(v.toDateTime().toMSecsSinceEpoch());
}


QVariant msToVariant(quint64 ms) {
	QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ms), QTimeZone::UTC);
	return QVariant(dateTime);
}


double fsec()
{
	quint64 now=currentMsecsSinceEpoch<quint64>();
	double out=now;
	out/=1000.0;
	return out;
}


}
}
