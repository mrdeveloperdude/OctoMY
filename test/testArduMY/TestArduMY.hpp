#ifndef TESTARDUMY_HPP
#define TESTARDUMY_HPP




#include <QTest>

class ActuatorConfig;
class ActuatorState;
class Actuator;
class ActuatorSet;


class TestArduMY:public QObject
{
	Q_OBJECT
private:

	ActuatorConfig randomConfig() const ;
	ActuatorConfig fuzzConfig() const ;

	ActuatorState randomState(const ActuatorConfig &c) const ;
	ActuatorState fuzzState() const ;

	Actuator randomActuator() const ;
	Actuator fuzzActuator() const ;

	ActuatorSet randomActuatorSet();
	ActuatorSet fuzzActuatorSet();


	void testActuatorFuzzConfigSerializer();
	void testActuatorFuzzConfigParser();
	void testActuatorFuzzConfigEquals();
	void testActuatorFuzzValueParser();
	void testActuatorFuzzValueSerializer();
	void testActuatorFuzzValueEquals();

private slots:

	void testMagicDetector();
	void testActuatorValue();
	void testValueConverter();
	void testActuatorRandomConfigParser();
	void testActuatorRandomConfigSerializer();
	void testActuatorRandomConfigEquals();
	void testActuatorRandomValueParser();

	void testActuatorRandomValueEquals();
	void testRepresentationBoundary();
	void testActuatorRandomValueSerializer();

	void testCommandParser();
	void testCommandSerializer();
};


#endif // TESTARDUMY_HPP
