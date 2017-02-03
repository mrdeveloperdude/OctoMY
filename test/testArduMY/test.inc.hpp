#ifndef TEST_INC_HPP
#define TEST_INC_HPP


#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"


#include "../arduino/ArduMYActuatorValueParser.hpp"
#include "../arduino/ActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QTest>


QString valueToString(const ActuatorValue &v, const ArduMYActuatorValueRepresentation &rep);
QString actuatorValueRepresentationToString(const ArduMYActuatorValueRepresentation &rep);
QString actuatorConfigToString(const ActuatorConfig &c);
QString actuatorStateToString(const ActuatorState &s, const ArduMYActuatorValueRepresentation &rep);
QString actuatorToString(const Actuator &a);
QString actuatorSetToString(const ActuatorSet &set);
void logLines(const QString ret);


#endif // TEST_INC_HPP
