#ifndef ARDUMYTYPECONVERSIONS_HPP
#define ARDUMYTYPECONVERSIONS_HPP
/** This file contains a bunch of handy to-from string/variant routines for the
 * ArduMY types.
 *
 * Usually in regular Qt code, this kind of routines are simply integrated in each
 * type, but since ArduMY is made to be independent from Qt, we need to maintain
 * these conversions separately, hence this file.
 *
  */


#ifdef OCTOMY_ARDUINO_TARGET
#error "WE ARE TRYING TO COMPILE ArdumyTypeConversions on AVR!"
#endif  // OCTOMY_ARDUINO_TARGET

#include "ardumy_arduino/actuator/ArduMYActuatorType.hpp"
#include "ardumy_arduino/actuator/ArduMYActuatorValue.hpp"
#include "ardumy_arduino/actuator/ArduMYActuatorValueRepresentation.hpp"
#include "ardumy_arduino/parser/ArduMYActuatorConfigParserStep.hpp"
#include "ardumy_arduino/parser/ArduMYActuatorValuesParserStep.hpp"
#include "ardumy_arduino/parser/ArduMYParserState.hpp"

struct ArduMYActuatorState;
struct ArduMYActuatorConfig;
struct ArduMYActuator;
class ArduMYActuatorSet;
class ArduMYCommandSerializer;
struct ArduMYCommandParser;


#include <QString>
#include <QVariant>

extern const ArduMYActuatorValueRepresentation DEFAULT_REPRESENTATION;
extern const ArduMYActuatorValuesParserStep DEFAULT_VALUE_PARSER_STEP;
extern const ArduMYActuatorConfigParserStep DEFAULT_CONFIG_PARSER_STEP;
extern const ArduMYActuatorType DEFAULT_ACTUATOR_TYPE;
extern const ArduMYActuatorValue DEFAULT_ACTUATOR_VALUE;
extern const ArduMYParserState DEFAULT_PARSER_STATE;


QString ardumyActuatorValueParserStepToString(const ArduMYActuatorValuesParserStep valuesParserStep);
ArduMYActuatorValuesParserStep ardumyActuatorValueParserStepFromString(const QString &string);

QString ardumyActuatorConfigParserStepToString(const ArduMYActuatorConfigParserStep configParserStep);
ArduMYActuatorConfigParserStep ardumyActuatorConfigParserStepFromString(const QString &string);

QString ardumyActuatorTypeToString(const ArduMYActuatorType actuatorType);
ArduMYActuatorType ardumyActuatorTypeFromString(const QString &string);

void ardumyActuatorValueFromVariant(ArduMYActuatorValue &actuatorValue, const QVariant &variant, const ArduMYActuatorValueRepresentation representation=DEFAULT_REPRESENTATION);
QVariant ardumyActuatorValueToVariant(const ArduMYActuatorValue &actuatorValue, const ArduMYActuatorValueRepresentation representation=DEFAULT_REPRESENTATION);

ArduMYActuatorValueRepresentation ardumyActuatorValueRepresentationFromString(const QString &string);
QString ardumyActuatorValueRepresentationToString(ArduMYActuatorValueRepresentation representation=DEFAULT_REPRESENTATION);

QString ardumyActuatorValueToString(const ArduMYActuatorValue &actuatorValue, const ArduMYActuatorValueRepresentation representation=DEFAULT_REPRESENTATION);
//QString ardumyActuatorValueToString(ArduMYActuatorValue value, ArduMYActuatorValueRepresentation representation=DEFAULT_REPRESENTATION);

QString ardumyParserStateToString(const ArduMYParserState parserState);
ArduMYParserState ardumyParserStateFromString(const QString &string);

QString ardumyActuatorConfigFlagsToString(const ArduMYActuatorConfig &actuatorConfig);
QString ardumyActuatorConfigToString(const ArduMYActuatorConfig &actuatorConfig);
QString ardumyActuatorStateFlagsToString(const ArduMYActuatorState &actuatorState);
QString ardumyActuatorStateToString(const ArduMYActuatorState &actuatorState, const ArduMYActuatorValueRepresentation representation=DEFAULT_REPRESENTATION);

QString ardumyActuatorToString(const ArduMYActuator &actuator);
QString ardumyActuatorSetToString(const ArduMYActuatorSet &actuatorSet);

QString ardumyActuatorNameToString(const ArduMYActuatorConfig &actuatorConfig);
void ardumyActuatorNameFromString(ArduMYActuatorConfig &actuatorConfig, const QString &string);

QString ardumyCommandSerializerToString(const ArduMYCommandSerializer &commandSerializer);
QString ardumyCommandParserToString(const ArduMYCommandParser &commandParser);

void diffArdumyActuatorState(const ArduMYActuatorState &actuatorStateA, const ArduMYActuatorState &actuatorStateB, const ArduMYActuatorValueRepresentation &representation);
void diffArdumyActuatorConfig(const ArduMYActuatorConfig &actuatorConfigA, const ArduMYActuatorConfig &actuaotorConfigB);
void diffArdumyActuator(const ArduMYActuator &actuatorA, const ArduMYActuator &actuatorB);
void diffArdumyActuatorSet(const ArduMYActuatorSet &actuatorSetA, const ArduMYActuatorSet &actuatorSetB);

#endif
// ARDUMYTYPECONVERSIONS_HPP
