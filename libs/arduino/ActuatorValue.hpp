#ifndef ACTUATORVALUE_HPP
#define ACTUATORVALUE_HPP

#include "ArduMY.hpp"

// This may waste a little memory, but I think the flexibliity and convenience it adds makes it worth it
union ActuatorValue {
	ActuatorValue()
		:quadWord(0) {

	}

	// The actual position for actuator. Bound by values of rangeStart and rangeSpan in config for all except bit.
	// + Interpreted as absolute position in non-continuous actuator
	// + Interpreted as accelleration for continuous actuator
	// + Interpreted as step in stepped actuator
	bool bit;				// 1 - bit int (boolean)
	uint8_t  byte;			// 8 - bit int
	uint16_t word;			// 16 - bit int
	uint32_t doubleWord;	// 32 - bit int
	uint64_t quadWord;		// 64 - bit int
	float singlePrecision;
	double doublePrecision;
};



#endif // ACTUATORVALUE_HPP
