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

public:

	RateCalculator(QString name="Unnamed");

	void countPacket(quint32 bytes);
};
#endif // RATECALCULATOR_HPP
