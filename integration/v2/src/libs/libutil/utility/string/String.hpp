#ifndef UTILITY_STRING_HPP
#define UTILITY_STRING_HPP

#include <QString>
#include <QVariant>
#include <QDateTime>

namespace utility
{

namespace string
{


QString humanReadableElapsedSeconds(long double seconds,int prec=2, bool prep=true,long double resolution=0.000001L);

QString formattedDateFromSeconds(long double seconds);
QString formattedDateFromMS(qint64 ms);
QString formattedDateFromMS(quint64 ms);

QString humanReadableSize(qint64 bytes,int prec=2);


template <typename T>
QString humanReadableElapsedMS(T ms,int prec=2,bool prep=true,long double resolution=0.000001L)
{
	return humanReadableElapsedSeconds(static_cast<long double>(ms) / 1000.0l, prec, prep, resolution);
}


template <typename T>
QString humanReadableSize(T bytes, int prec=2)
{
	return humanReadableSize(static_cast<qint64>(bytes), prec);
}



}

}

#endif
// UTILITY_STRING_HPP

