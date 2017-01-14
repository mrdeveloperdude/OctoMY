#include "ActuatorSet.hpp"



bool operator== (const ActuatorSet &a, const ActuatorSet &b)
{
	return a.isEqual(b);
}


bool operator!= (const ActuatorSet &a, const ActuatorSet &b)
{
	return !a.isEqual(b);
}
