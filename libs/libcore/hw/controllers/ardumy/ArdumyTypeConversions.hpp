#ifndef ARDUMYTYPECONVERSIONS_HPP
#define ARDUMYTYPECONVERSIONS_HPP

#include "../arduino/ArduMYActuatorType.hpp"
#include "../arduino/ArduMYActuatorValueRepresentation.hpp"
#include "../arduino/ArduMYActuatorValue.hpp"
#include "../arduino/ArduMYActuatorState.hpp"
#include "../arduino/ArduMYActuatorConfig.hpp"
#include "../arduino/ArduMYActuator.hpp"
#include "../arduino/ArduMYActuatorSet.hpp"

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

QString ardumyActuatorTypeToString(const ArduMYActuatorType &s);
ArduMYActuatorType ardumyActuatorTypeFromString(const QString &s);
void ardumyActuatorValueFromVariant(ArduMYActuatorValue &val, const QVariant &variant, const ArduMYActuatorValueRepresentation &representation=DEFAULT_REPRESENTATION);
QVariant ardumyActuatorValueToVariant(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);
ArduMYActuatorValueRepresentation ardumyActuatorValueRepresentationFromString(const QString &s);

QString ardumyActuatorValueRepresentationToString(const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);
QString ardumyActuatorValueToString(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);
QString ardumyActuatorConfigFlagsToString(const ArduMYActuatorConfig &c);
QString ardumyActuatorConfigToString(const ArduMYActuatorConfig &c);
QString ardumyActuatorStateFlagsToString(const ArduMYActuatorState &s);
QString ardumyActuatorStateToString(const ArduMYActuatorState &s, const ArduMYActuatorValueRepresentation &rep=DEFAULT_REPRESENTATION);
QString ardumyActuatorToString(const ArduMYActuator &a);
QString ardumyActuatorSetToString(const ArduMYActuatorSet &set);

QString ardumyActuatorNameToString(const ArduMYActuatorConfig &c);
void ardumyActuatorNameFromString(ArduMYActuatorConfig &c ,const QString &string);

#endif // ARDUMYTYPECONVERSIONS_HPP
