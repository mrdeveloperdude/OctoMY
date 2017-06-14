#ifndef FPSCALCULATOR_HPP
#define FPSCALCULATOR_HPP

#include <QString>


class FPSCalculator
{
private:
	const QString name;
	const quint64 logInterval;
	quint64 frameAccumulator;
	quint64 lastTime;

public:
	explicit FPSCalculator(QString name, quint64 logInterval=1000);
	virtual ~FPSCalculator();
public:

	void update();
};

#endif // FPSCALCULATOR_HPP
