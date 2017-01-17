#ifndef ACTUATORSTATE_HPP
#define ACTUATORSTATE_HPP


#include "ActuatorValueRepresentation.hpp"


#include "ActuatorType.hpp"
#include "ActuatorValue.hpp"

#include "ActuatorFlags.hpp"


struct ActuatorState {

	uint32_t flags;

	ACTUATOR_FLAG_SELECTOR(isLimp,					setLimp,				0 )
	ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				1 )

	ActuatorValue value;

	bool isEqual(const ActuatorState &other, ActuatorValueRepresentation representation=ActuatorValueRepresentation::REPRESENTATION_COUNT) const
	{

		// Mask to remove ephemeral parts of flags (currently only isDirty)
		const uint16_t temp=(1<<1);
		const uint16_t mask=(~temp);
		if( (flags & mask) != (other.flags & mask)) {
			return false;
		}

		if(!value.isEqual(other.value,representation)) {
			return false;
		}

		// By not failig until now we win!
		return true;
	}
};


bool operator== (const ActuatorState &a, const ActuatorState &b);
bool operator!= (const ActuatorState &a, const ActuatorState &b);

#endif // ACTUATORSTATE_HPP
