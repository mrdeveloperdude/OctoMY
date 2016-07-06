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

#ifdef USE_FEATURE_BUFFER_HONEYPOT
#define BUFFER_HONEYPOT BufferHoneyPot bhp_(true)
#else
#define BUFFER_HONEYPOT
#endif


#define OC_METHODGATE()  { if(((const long long unsigned int)this)<1024){ qWarning()<<"this<1024"; } } BUFFER_HONEYPOT
#define OC_FUNCTIONGATE()  { } BUFFER_HONEYPOT

#define OC_TIMEOUTWARN(to) ScopedTimer(Q_FUNC_INFO,(qint64)to);

#define OC_ASSERT(a) { const bool c=(a); if(!c){ qWarning()<< "OC_ASSERT FAILED: " #a; } }


#define OC_CONTYPE (Qt::ConnectionType) (Qt::QueuedConnection | Qt::UniqueConnection)



#define WEB_TIMEOUT (4000)

QDebug operator<< (QDebug d, void *p);
const QString 	operator+ ( const QString & s, void *p );


#endif
//STANDARD_HPP
