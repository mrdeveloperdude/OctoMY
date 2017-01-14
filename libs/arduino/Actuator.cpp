#include "Actuator.hpp"


bool operator== (const ActuatorConfig &a, const ActuatorConfig &b)
{
	return a.isEqual(b);
}


bool operator!= (const ActuatorConfig &a, const ActuatorConfig &b)
{
	return !a.isEqual(b);
}



bool operator== (const ActuatorState &a, const ActuatorState &b)
{
	return a.isEqual(b);
}


bool operator!= (const ActuatorState &a, const ActuatorState &b)
{
	return !a.isEqual(b);
}




bool operator== (const Actuator &a, const Actuator &b)
{
	return a.isEqual(b);
}


bool operator!= (const Actuator &a, const Actuator &b)
{
	return !a.isEqual(b);
}
