#ifndef ACTUATORSET_HPP
#define ACTUATORSET_HPP

#include "Actuator.hpp"
#include "DynamicArray.hpp"


#include "ArduMY.hpp"

class ActuatorSet: public DynamicArray<Actuator>
{
public:
	ActuatorSet(unsigned int initialSize=0)
		: DynamicArray<Actuator>(initialSize)
	{
		//cout<< "ActuatorSet CTOR\n";
	}

	virtual ~ActuatorSet()
	{

	}

};

#endif // ACTUATORSET_HPP
