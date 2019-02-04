#ifndef ARDUMYACTUATORVALUE_HPP
#define ARDUMYACTUATORVALUE_HPP

#include "ardumy_arduino/ArduMY.hpp"

#include "ardumy_arduino/ArduMYActuatorValueRepresentation.hpp"

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

	bool isEqual(const ArduMYActuatorValue &other, ArduMYActuatorValueRepresentation representation=ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT) const {
		switch(representation) {
		case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
			if(bit != other.bit) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
			if(byte != other.byte) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
			if(word != other.word) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
			if(doubleWord != other.doubleWord) {
				return false;
			}

		}
		break;
		//Default to strictest possible equality when representation is unknown
		default:
		case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT):
		case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
			if(quadWord != other.quadWord) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
			if(singlePrecision != other.singlePrecision) {
				return false;
			}

		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
			if(doublePrecision != other.doublePrecision) {
				return false;
			}
		}
		break;
		}


		// By not failig until now we win!
		return true;
	}


	qreal toFloat(ArduMYActuatorValueRepresentation representation=ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT) {
		qreal ret=0.0f;

		switch(representation) {
		case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
			ret=bit?1.0f:0.0f;
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
			ret=((qreal)byte)/0xFF;
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
			ret=((qreal)word)/0xFFFF;
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
			ret=((qreal)doubleWord)/0xFFFFFFFF;
		}
		break;
		//Default to strictest possible equality when representation is unknown
		default:
		case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT):
		case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
			ret=((qreal)quadWord)/0xFFFFFFFFFFFFFFFF;
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
			ret=singlePrecision;
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
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
