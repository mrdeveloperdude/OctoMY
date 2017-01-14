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

	void singleActuatorConfigParserRun(ActuatorConfig &c) const ;
	void singleActuatorConfigSerializerRun(ActuatorConfig &c) const ;
	void singleActuatorConfigEqualsRun(const ActuatorConfig &a) const;
	void singleActuatorSetParserRun(ActuatorSet &set) const;
	void singleActuatorSetSerializerRun(ActuatorSet &set) const;

	void testActuatorFuzzConfigSerializer();
	void testActuatorFuzzConfigParser();
	void testActuatorFuzzConfigEquals();
	void testActuatorFuzzValueParser();




	void testMagicDetector();
	void testActuatorRandomConfigParser();
	void testActuatorRandomConfigSerializer();
	void testActuatorRandomConfigEquals();
	void testActuatorRandomValueParser();

	void testRepresentationBoundary();
	void testCommandParser();


private slots:


	void testActuatorRandomValueSerializer();
	/*
	void testMagicDetector();
	void testActuatorRandomConfigParser();
	void testActuatorRandomConfigSerializer();
	void testActuatorRandomConfigEquals();
	void testActuatorRandomValueParser();
	void testActuatorRandomValueSerializer();
	void testRepresentationBoundary();
	void testCommandParser();
	*/
};


#endif // TESTARDUMY_HPP
