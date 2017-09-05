#ifndef STANDARD_HPP
#define STANDARD_HPP

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#endif



#include <QDebug>
#include <QString>
#include <QtMath>
#include <new>

#ifdef USE_FEATURE_BUFFER_HONEYPOT
#define BUFFER_HONEYPOT BufferHoneyPot bhp_(true)
#else
#define BUFFER_HONEYPOT
#endif

#define OC_LOWEST_POSSIBLE_ADDRESS (0xd0000)

#define OC_METHODGATE()  { if(((const long long unsigned int)this)<OC_LOWEST_POSSIBLE_ADDRESS){ qWarning()<<"ERROR: this < "<<OC_LOWEST_POSSIBLE_ADDRESS<<", which likely means nullptr access"; } } BUFFER_HONEYPOT
#define OC_FUNCTIONGATE()  { } BUFFER_HONEYPOT

#define OC_TIMEOUTWARN(to) ScopedTimer(Q_FUNC_INFO,(qint64)to);

#define OC_ASSERT(a) { const bool c=(a); if(!c){ qWarning()<< "OC_ASSERT FAILED: " #a; } }

#define OC_CONTYPE (Qt::ConnectionType) (Qt::QueuedConnection | Qt::UniqueConnection)

#define OC_COLLECT_AT_MOST (1024*100)

#define OC_NEW new(std::nothrow)

#define WEB_TIMEOUT (4000)

QDebug operator<< (QDebug d, void *p);
const QString 	operator+ ( const QString & s, void *p );


#endif
//STANDARD_HPP
