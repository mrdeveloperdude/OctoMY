#ifndef TESTARDUMY_HPP
#define TESTARDUMY_HPP




#include <QTest>

class ArduMYActuatorConfig;
class ArduMYActuatorState;
class ArduMYActuator;
class ArduMYActuatorSet;


class TestArduMY:public QObject
{
	Q_OBJECT
private:

	ArduMYActuatorConfig randomConfig() const ;
	ArduMYActuatorConfig fuzzConfig() const ;

	ArduMYActuatorState randomState(const ArduMYActuatorConfig &c) const ;
	ArduMYActuatorState fuzzState() const ;

	ArduMYActuator randomActuator() const ;
	ArduMYActuator fuzzActuator() const ;

	ArduMYActuatorSet randomActuatorSet();
	ArduMYActuatorSet fuzzActuatorSet();
	ArduMYActuatorSet fixedActuatorSet();


	void testActuatorFuzzConfigSerializer();
	void testActuatorFuzzConfigParser();
	void testActuatorFuzzConfigEquals();
	void testActuatorFuzzValueParser();
	void testActuatorFuzzValueSerializer();
	void testActuatorFuzzValueEquals();


	void testMagicDetector();
	void testDynamicArrayFundamental();
	void testDynamicArrayPOD();
	void testActuatorValue();
	void testValueConverter();
	void testActuatorRandomConfigParser();
	void testActuatorRandomConfigSerializer();
	void testActuatorRandomConfigEquals();
	void testActuatorRandomValueParser();
	void testActuatorRandomValueEquals();
	void testRepresentationBoundary();
	void testCommandParser();
	void testCommandSerializer();
private slots:


	void testActuatorRandomValueSerializer();
};


#endif // TESTARDUMY_HPP
