#include "ArduMYActuatorConfig.hpp"



bool operator== (const ArduMYActuatorConfig &a, const ArduMYActuatorConfig &b)
{
	return a.isEqual(b);
}


bool operator!= (const ArduMYActuatorConfig &a, const ArduMYActuatorConfig &b)
{
	return !a.isEqual(b);
}

