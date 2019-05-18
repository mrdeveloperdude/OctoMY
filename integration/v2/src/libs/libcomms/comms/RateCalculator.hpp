#ifndef RATECALCULATOR_HPP
#define RATECALCULATOR_HPP

#include <QString>

/**
 * @brief The RateCalculatorLogger class is used to allow multiple loggings occur for a rate calculator without getting in it's way
 */
struct RateCalculatorLogger
{
	quint64 mLastLog;
	quint64 mLogInterval;
	quint64 mCountLog;
	quint64 mPacketRate;
	quint64 mByteRate;
};

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

private:
	void log(RateCalculatorLogger &logger);

};

const QDebug &operator<<(QDebug &d, const RateCalculator &rc);


#endif
// RATECALCULATOR_HPP
