#ifndef STRESSCAPABILITIES_HPP
#define STRESSCAPABILITIES_HPP

#include "../common/TestCommon.hpp"

#include "Stress.hpp"

class StressCapabilities:public QObject
{
	Q_OBJECT
private:
	Stress mStress;

private slots:
	void stress();

};


#endif // STRESSCAPABILITIES_HPP
