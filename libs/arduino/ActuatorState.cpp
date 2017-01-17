#include "ActuatorState.hpp"



bool operator== (const ActuatorState &a, const ActuatorState &b)
{
	return a.isEqual(b);
}


bool operator!= (const ActuatorState &a, const ActuatorState &b)
{
	return !a.isEqual(b);
}
