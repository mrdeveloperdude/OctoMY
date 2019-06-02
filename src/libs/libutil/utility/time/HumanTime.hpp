#ifndef UTILITY_HUMANTIME_HPP
#define UTILITY_HUMANTIME_HPP

#include <QString>
#include <QVariant>
#include <QDateTime>

namespace utility
{

namespace time
{

void delay( int millisecondsToWait );


template<typename T>
T currentMsecsSinceEpoch();

double fsec();

quint64 variantToMs(QVariant v);
QVariant msToVariant(quint64 ms);


template<typename T>
T currentMsecsSinceEpoch()
{
	return static_cast<T>(QDateTime::currentMSecsSinceEpoch());
}

}

}

#endif
// UTILITY_HUMANTIME_HPP

