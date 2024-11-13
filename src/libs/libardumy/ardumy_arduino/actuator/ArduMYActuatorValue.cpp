#include "ardumy_arduino/actuator/ArduMYActuatorValue.hpp"


bool operator== (const ArduMYActuatorValue &a, const ArduMYActuatorValue &b)
{
	return a.isEqual(b);
}

bool operator!= (const ArduMYActuatorValue &a, const ArduMYActuatorValue &b)
{
	return !a.isEqual(b);
}

