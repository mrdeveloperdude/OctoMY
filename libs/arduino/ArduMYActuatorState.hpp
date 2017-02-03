#ifndef ARDUMYACTUATORSTATE_HPP
#define ARDUMYACTUATORSTATE_HPP


#include "ArduMYActuatorValueRepresentation.hpp"


#include "ArduMYActuatorType.hpp"
#include "ArduMYActuatorValue.hpp"

#include "ArduMYActuatorFlags.hpp"


struct ArduMYActuatorState {

	uint32_t flags;

	ARDUMY_ACTUATOR_FLAG_SELECTOR(isLimp,					setLimp,				0 )
	ARDUMY_ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				1 )

	ArduMYActuatorValue value;

	bool isEqual(const ArduMYActuatorState &other, ArduMYActuatorValueRepresentation representation=ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT) const
	{

		// Mask to remove ephemeral parts of flags (currently only isDirty)
		const uint16_t temp=(1<<1);
		const uint16_t mask=(~temp);
		if( (flags & mask) != (other.flags & mask)) {
			qDebug()<<" FLAGS DIFFER:";
			return false;
		}

		if(!value.isEqual(other.value,representation)) {
			qDebug()<<" VALUE DIFFER";
			return false;
		}

		// By not failig until now we win!
		return true;
	}
};


bool operator== (const ArduMYActuatorState &a, const ArduMYActuatorState &b);
bool operator!= (const ArduMYActuatorState &a, const ArduMYActuatorState &b);

#endif // ACTUATORSTATE_HPP
