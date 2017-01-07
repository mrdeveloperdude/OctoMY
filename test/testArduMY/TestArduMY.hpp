#ifndef TESTARDUMY_HPP
#define TESTARDUMY_HPP

#include "../arduino/CommandParser.hpp"

#include <QTest>


class TestArduMY:public QObject
{
	Q_OBJECT
private:

	const bool percentChance(quint8 pct) const ;
	ActuatorConfig randomConfig() const ;
	void singleServoConfigRun(const ActuatorConfig &c) const ;

private slots:
	void testMagicDetector();
	void testServoConfigParser();
	void testServoPositionParser();

};


#endif // TESTARDUMY_HPP

