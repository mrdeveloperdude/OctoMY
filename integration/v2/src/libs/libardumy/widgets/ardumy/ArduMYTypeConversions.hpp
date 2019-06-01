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

#include "ardumy_arduino/ArduMYActuatorType.hpp"
#include "ardumy_arduino/ArduMYActuatorValueRepresentation.hpp"
#include "ardumy_arduino/ArduMYActuatorValue.hpp"
#include "ardumy_arduino/ArduMYActuatorConfigParserStep.hpp"
#include "ardumy_arduino/ArduMYActuatorValuesParserStep.hpp"
#include "ardumy_arduino/ArduMYParserState.hpp"

class ArduMYActuatorState;
class ArduMYActuatorConfig;
class ArduMYActuator;
class ArduMYActuatorSet;
class ArduMYCommandSerializer;
class ArduMYCommandParser;


#include <QString>
#include <QVariant>


/*
NOTE: All of these convenience conversion routines aare kept in a separate file so that it is not compiled into the code that will run on the MCU. It is only useful from the Qt side
*/

/*
QString ardumyActuatorValueRepresentationToString(ArduMYActuatorValueRepresentation s);
ArduMYActuatorValueRepresentation ardumyActuatorValueRepresentationFromString(QString s);

QString ardumyActuatorValueToString(ArduMYActuatorValue val, ArduMYActuatorValueRepresentation representation=REPRESENTATION_COUNT);

QString ardumyActuatorStateToString(ArduMYActuatorState &state, ArduMYActuatorValueRepresentation representation=REPRESENTATION_COUNT);
QString ardumyActuatorConfigToString(ArduMYActuatorConfig &config);
*/



extern const ArduMYActuatorValueRepresentation DEFAULT_REPRESENTATION;

extern const ArduMYActuatorValuesParserStep DEFAULT_VALUE_PARSER_STEP;
QString ardumyActuatorValueParserStepToString(const ArduMYActuatorValuesParserStep &s);
ArduMYActuatorValuesParserStep ardumyActuatorValueParserStepFromString(const QString &s);

extern const ArduMYActuatorConfigParserStep DEFAULT_CONFIG_PARSER_STEP;
QString ardumyActuatorConfigParserStepToString(const ArduMYActuatorConfigParserStep &s);
ArduMYActuatorConfigParserStep ardumyActuatorConfigParserStepFromString(const QString &s);

extern const ArduMYActuatorType DEFAULT_ACTUATOR_TYPE;
QString ardumyActuatorTypeToString(const ArduMYActuatorType &s);
ArduMYActuatorType ardumyActuatorTypeFromString(const QString &s);

extern const ArduMYActuatorValue DEFAULT_ACTUATOR_VALUE;
void ardumyActuatorValueFromVariant(ArduMYActuatorValue &val, const QVariant &variant, const ArduMYActuatorValueRepresentation &representation=DEFAULT_REPRESENTATION);
QVariant ardumyActuatorValueToVariant(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);

ArduMYActuatorValueRepresentation ardumyActuatorValueRepresentationFromString(const QString &s);
QString ardumyActuatorValueRepresentationToString(const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);

QString ardumyActuatorValueToString(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);

extern const ArduMYParserState DEFAULT_PARSER_STATE;
QString ardumyParserStateToString(const ArduMYParserState &ps);
ArduMYParserState ardumyParserStateFromString(const QString &s);


QString ardumyActuatorConfigFlagsToString(const ArduMYActuatorConfig &c);
QString ardumyActuatorConfigToString(const ArduMYActuatorConfig &c);
QString ardumyActuatorStateFlagsToString(const ArduMYActuatorState &s);
QString ardumyActuatorStateToString(const ArduMYActuatorState &s, const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);
QString ardumyActuatorToString(const ArduMYActuator &a);
QString ardumyActuatorSetToString(const ArduMYActuatorSet &set);

QString ardumyActuatorNameToString(const ArduMYActuatorConfig &c);
void ardumyActuatorNameFromString(ArduMYActuatorConfig &c ,const QString &string);

QString ardumyCommandSerializerToString(const ArduMYCommandSerializer &cs);
QString ardumyCommandParserToString(const ArduMYCommandParser &cp);

void diffArdumyActuatorState(const ArduMYActuatorState &a, const ArduMYActuatorState &b, const ArduMYActuatorValueRepresentation &rep);
void diffArdumyActuatorConfig(const ArduMYActuatorConfig &a, const ArduMYActuatorConfig &b);
void diffArdumyActuator(const ArduMYActuator &a, const ArduMYActuator &b);
void diffArdumyActuatorSet(const ArduMYActuatorSet &setA, const ArduMYActuatorSet &setB);

#endif
// ARDUMYTYPECONVERSIONS_HPP
