#include "ardumy_arduino/actuator/ArduMYActuator.hpp"



bool operator== (const ArduMYActuator &a, const ArduMYActuator &b)
{
	return a.isEqual(b);
}


bool operator!= (const ArduMYActuator &a, const ArduMYActuator &b)
{
	return !a.isEqual(b);
}
