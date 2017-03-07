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

// TODO: Fuzzing isn't really practical before the fuzz data is a little less brutal.
	void testActuatorFuzzConfigSerializer();
	void testActuatorFuzzConfigParser();
	void testActuatorFuzzConfigEquals();
	void testActuatorFuzzValueParser();
	void testActuatorFuzzValueSerializer();
	void testActuatorFuzzValueEquals();


private slots:

	void testActuatorRandomConfigEquals();
	void testActuatorRandomConfigParser();
	void testActuatorRandomConfigSerializer();
	void testActuatorRandomValueEquals();
	void testActuatorRandomValueParser();
	void testActuatorRandomValueSerializer();
	void testActuatorValue();
	void testCommandParser();
	void testCommandSerializer();
	void testDynamicArrayFundamental();
	void testDynamicArrayPOD();
	void testMagicDetector();
	void testRepresentationBoundary();
	void testToFromString();
	void testValueConverter();

};


#endif // TESTARDUMY_HPP
