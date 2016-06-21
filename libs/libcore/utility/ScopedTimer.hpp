#ifndef SCOPEDTIMER_HPP
#define SCOPEDTIMER_HPP

#include <QString>

#define SCOPED_TIMER_SYMBOL "⌚"
//#define SCOPED_TIMER_SYMBOL "TIMER"

class ScopedTimer{
	private:
		qint64 startTime;
		qint64 now;
		qint64 interval;
		bool doLog;
		//		bool human;
		QString name;
		QString fmt;
		qint64 timeout;
	private:
		void update();
	public:
		explicit ScopedTimer(QString name, bool log=true, bool human=true, QString fmt="yyyy-MM-dd hh:mm:ss.zzz");
		explicit ScopedTimer(QString name, qint64 timeout, bool human=true, QString fmt="yyyy-MM-dd hh:mm:ss.zzz");
		virtual ~ScopedTimer();
		qint64 getInterval( bool up=true);
		QString getIntervalCompleteString(QString prefix=SCOPED_TIMER_SYMBOL " INTERVAL: ",  bool up=true);
		QString getIntervalHuman(int prec=2,bool prep=true, bool up=true);
		QString getTimeFormated(QString fmt="yyyy-MM-dd hh:mm:ss.zzz", bool up=true);
};


#endif // SCOPEDTIMER_HPP
