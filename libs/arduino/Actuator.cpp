#include "Actuator.hpp"



bool operator== (const Actuator &a, const Actuator &b)
{
	return a.isEqual(b);
}


bool operator!= (const Actuator &a, const Actuator &b)
{
	return !a.isEqual(b);
}
