#include "ArduMYActuatorSet.hpp"



bool operator== (const ArduMYActuatorSet &a, const ArduMYActuatorSet &b)
{
	return a.isEqual(b);
}


bool operator!= (const ArduMYActuatorSet &a, const ArduMYActuatorSet &b)
{
	return !a.isEqual(b);
}
