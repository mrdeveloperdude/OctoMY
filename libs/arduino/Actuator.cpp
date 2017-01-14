#include "Actuator.hpp"


bool operator== (const ActuatorConfig &a, const ActuatorConfig &b)
{
	return a.isEqual(b);
}


bool operator!= (const ActuatorConfig &a, const ActuatorConfig &b)
{
	return !a.isEqual(b);
}
