#ifndef ACTUATORVALUE_HPP
#define ACTUATORVALUE_HPP

#include "ArduMY.hpp"

#include "ActuatorValueRepresentation.hpp"

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

	bool isEqual(const ActuatorValue &other, ActuatorValueRepresentation representation=ActuatorValueRepresentation::REPRESENTATION_COUNT) const {
		switch(representation) {
		case(ActuatorValueRepresentation::BIT): {
			if(bit != other.bit) {
				return false;
			}

		}
		break;
		case(ActuatorValueRepresentation::BYTE): {
			if(byte != other.byte) {
				return false;
			}

		}
		break;
		case(ActuatorValueRepresentation::WORD): {
			if(word != other.word) {
				return false;
			}

		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_WORD): {
			if(doubleWord != other.doubleWord) {
				return false;
			}

		}
		break;
		//Default to strictest possible equality when representation is unknown
		default:
		case(ActuatorValueRepresentation::REPRESENTATION_COUNT):
		case(ActuatorValueRepresentation::QUAD_WORD): {
			if(quadWord != other.quadWord) {
				return false;
			}

		}
		break;
		case(ActuatorValueRepresentation::SINGLE_FLOAT): {
			if(singlePrecision != other.singlePrecision) {
				return false;
			}

		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
			if(doublePrecision != other.doublePrecision) {
				return false;
			}
		}
		break;
		}


		// By not failig until now we win!
		return true;
	}
};


bool operator== (const ActuatorValue &a, const ActuatorValue &b);
bool operator!= (const ActuatorValue &a, const ActuatorValue &b);


#endif // ACTUATORVALUE_HPP
