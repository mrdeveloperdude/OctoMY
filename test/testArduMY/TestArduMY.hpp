#ifndef TESTARDUMY_HPP
#define TESTARDUMY_HPP




#include <QTest>

class ActuatorConfig;


class TestArduMY:public QObject
{
	Q_OBJECT
private:

	ActuatorConfig randomConfig() const ;
	void singleActuatorConfigParserRun(ActuatorConfig &c) const ;
	void singleActuatorConfigSerializerRun(ActuatorConfig &c) const ;

private slots:
	void testMagicDetector();
	void testActuatorConfigSerializer();
	void testActuatorConfigParser();
	void testActuatorValueParser();
	void testCommandParser();

};


#endif // TESTARDUMY_HPP

