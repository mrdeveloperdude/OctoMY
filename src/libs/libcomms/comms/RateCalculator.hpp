#ifndef RATECALCULATOR_HPP
#define RATECALCULATOR_HPP

#include <QString>

class RateCalculator
{

public:
	QString mName;
	quint64 mLast; // Last time data passed
	quint64 mCount; // Number of packets passed
	quint64 mBytes; // Number of bytes passed
	quint64 mLastLog; // Last time we logged rate
	quint64 mCountLog; // Number of packets passed since last log
	quint64 mBytesLog; // Number of bytes passed since last log
	quint64 mLogInterval; // Interval for logging. 0 means no logging

public:

	RateCalculator(QString name="Unnamed", const quint64 logInterval=0);

	void countPacket(quint32 bytes=0, quint64 now=0);

	QString operator()(RateCalculator &rc);
};

const QDebug &operator<<(QDebug &d, const RateCalculator &rc);


#endif // RATECALCULATOR_HPP
