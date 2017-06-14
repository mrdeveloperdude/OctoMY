#include "ArduMYActuatorState.hpp"



bool operator== (const ArduMYActuatorState &a, const ArduMYActuatorState &b)
{
	return a.isEqual(b);
}


bool operator!= (const ArduMYActuatorState &a, const ArduMYActuatorState &b)
{
	return !a.isEqual(b);
}
