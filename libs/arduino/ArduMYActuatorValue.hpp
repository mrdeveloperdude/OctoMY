#ifndef ARDUMYACTUATORVALUE_HPP
#define ARDUMYACTUATORVALUE_HPP

#include "ArduMY.hpp"

#include "ArduMYActuatorValueRepresentation.hpp"

// This may waste a little memory, but I think the flexibliity and convenience it adds makes it worth it
union ArduMYActuatorValue {
	ArduMYActuatorValue()
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

	bool isEqual(const ArduMYActuatorValue &other, ArduMYActuatorValueRepresentation representation=ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT) const {
		switch(representation) {
		case(ArduMYActuatorValueRepresentation::BIT): {
			if(bit != other.bit) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::BYTE): {
			if(byte != other.byte) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::WORD): {
			if(word != other.word) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
			if(doubleWord != other.doubleWord) {
				return false;
			}

		}
		break;
		//Default to strictest possible equality when representation is unknown
		default:
		case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT):
		case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
			if(quadWord != other.quadWord) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
			if(singlePrecision != other.singlePrecision) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
			if(doublePrecision != other.doublePrecision) {
				return false;
			}
		}
		break;
		}


		// By not failig until now we win!
		return true;
	}


	qreal toFloat(ArduMYActuatorValueRepresentation representation=ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT) {
		qreal ret=0.0f;

		switch(representation) {
		case(ArduMYActuatorValueRepresentation::BIT): {
			ret=bit?1.0f:0.0f;
		}
		break;
		case(ArduMYActuatorValueRepresentation::BYTE): {
			ret=((qreal)byte)/0xFF;
		}
		break;
		case(ArduMYActuatorValueRepresentation::WORD): {
			ret=((qreal)byte)/0xFFFF;
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
			ret=((qreal)byte)/0xFFFFFFFF;
		}
		break;
		//Default to strictest possible equality when representation is unknown
		default:
		case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT):
		case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
			ret=((qreal)byte)/0xFFFFFFFFFFFFFFFF;
		}
		break;
		case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
			ret=singlePrecision;
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
			ret=doublePrecision;
		}
		break;
		}
		return ret;
	}
};


bool operator== (const ArduMYActuatorValue &a, const ArduMYActuatorValue &b);
bool operator!= (const ArduMYActuatorValue &a, const ArduMYActuatorValue &b);


#endif // ARDUMYACTUATORVALUE_HPP
