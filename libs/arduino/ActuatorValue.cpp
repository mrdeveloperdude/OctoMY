#include "ActuatorValue.hpp"


bool operator== (const ActuatorValue &a, const ActuatorValue &b)
{
	return a.isEqual(b);
}

bool operator!= (const ActuatorValue &a, const ActuatorValue &b)
{
	return !a.isEqual(b);
}

