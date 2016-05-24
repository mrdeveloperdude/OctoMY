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


#define WWMETHODGATE()  { if(0==this){ qWarning()<<"0==this"; } } BUFFER_HONEYPOT
#define WWFUNCTIONGATE()  { } BUFFER_HONEYPOT

#define WWTIMEOUTWARN(to) ScopedTimer(Q_FUNC_INFO,(qint64)to);

#define WWASSERT(a) { const bool c=(a); if(!c){ qWarning()<< "WWASSERT FAILED: " #a; } }


#define WWCONTYPE (Qt::ConnectionType) (Qt::QueuedConnection | Qt::UniqueConnection)





#define OC_METHODGATE()		WWMETHODGATE()
#define OC_FUNCTIONGATE()	WWFUNCTIONGATE()
#define OC_TIMEOUTWARN(to)	WWTIMEOUTWARN(to)
#define OC_ASSERT(a)		WWASSERT(a)
#define OC_CONTYPE			WWCONTYPE



#define WEB_TIMEOUT (4000)

QDebug operator<< (QDebug d, void *p);
const QString 	operator+ ( const QString & s, void *p );


#endif
//STANDARD_HPP
