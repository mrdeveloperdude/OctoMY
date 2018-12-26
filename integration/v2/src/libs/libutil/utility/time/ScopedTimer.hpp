#ifndef SCOPEDTIMER_HPP
#define SCOPEDTIMER_HPP

#include <QString>

#define SCOPED_TIMER_SYMBOL "⌚"
//#define SCOPED_TIMER_SYMBOL "TIMER"


enum ScopedTimerFormat: quint8 {
	STF_NONE = 0
	, STF_NAME = 0x1
	, STF_TIME = 0x2
	, STF_HUMAN_ELAPSED = 0x4
	, STF_MILLIS_ELAPSED = 0x8
	, STF_ELAPSED = STF_HUMAN_ELAPSED | STF_MILLIS_ELAPSED | 0x16
	, STF_AUTO = 0x32
};

class ScopedTimer
{
private:
	qint64 mStartTime;
	qint64 mNow;
	qint64 mInterval;
	bool mDoLog;
	//		bool human;
	QString mName;
	QString mFormatString;
	qint64 mTimeout;
	ScopedTimerFormat mFormat;
private:
	void update();

	void doAutoFormat();
public:
	explicit ScopedTimer(QString mName = "", bool log = true, ScopedTimerFormat format = STF_AUTO, QString mFormatString="yyyy-MM-dd hh:mm:ss.zzz");
	explicit ScopedTimer(QString mName, qint64 mTimeout, ScopedTimerFormat format = STF_AUTO, QString mFormatString="yyyy-MM-dd hh:mm:ss.zzz");
	virtual ~ScopedTimer();
	qint64 getInterval(bool up = true);
	QString getIntervalCompleteString(QString prefix = SCOPED_TIMER_SYMBOL " INTERVAL: ",  bool up=true);
	QString getIntervalHuman(int prec = 2, bool prep = true, bool up = true);
	QString getTimeFormated(QString mFormatString = "yyyy-MM-dd hh:mm:ss.zzz", bool up = true);
};


#endif // SCOPEDTIMER_HPP
