#ifndef UTILITY_STRING_HPP
#define UTILITY_STRING_HPP

#include <QString>
#include <QVariant>
#include <QDateTime>

#include <QCryptographicHash>
#include <QNetworkReply>
#include <QAbstractSocket>
#include <QMutex>

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




class HandleCounter
{
private:
	quint64 handleCounter;
	QMap<Qt::HANDLE, quint64> handleMap;
	QMutex handleMutex;

public:

	HandleCounter();
	QString handleCounterString(Qt::HANDLE h);

};




size_t snprint_hex(char *dst, size_t size, const uint8_t *pbtData, const size_t szBytes);
QString toHEX(const uint8_t *pbtData, const size_t szBytes);
QString hexdump(void *mem, unsigned int len, QString topic="");
QString humanReadablize(QString orig, int space=4, QString sep="-");
bool isPositiveInteger(QString s);
QString perror();
qint64 parseDate(QString str,QString fmt="yyyy-MM-ddThh:mm:ss");
QString toHash(QString input, QCryptographicHash::Algorithm algo=QCryptographicHash::Sha1, bool upperCase=true);
QString nameToNormalForm(QString name);
QString networkErrorToString( QNetworkReply::NetworkError ne);
QString socketErrorToString( QAbstractSocket::SocketError se);
QString padstring(int level, QString base=" ");
QString toSoundex(QString in);
int levenshtein_distance(const QString &s1, const QString  &s2);
QString handleCounterString(Qt::HANDLE h);
QString currentThreadID();



}

}

#endif
// UTILITY_STRING_HPP

