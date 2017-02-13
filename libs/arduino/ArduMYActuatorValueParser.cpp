#include "ArduMYActuatorValueParser.hpp"

#include "CommandParser.hpp"

#include "ArduMY.hpp"

ArduMYActuatorValueParser::ArduMYActuatorValueParser()
	: ArduMYActuatorValueSerializerBase()
{

}

/*
Since the data representation for actuators may vary, we will batch actuator vlues by representation.
The main benefit of this approach is allowing for sub-byte sized values. Currently this is only employed
for actuators with bit representation, and for configurations with mostly bit actuators the data savings can be substantial (1/8 of the data is used at best)

The batches are ordered by smallest to largest integer, then smallest to largest float.
The exact batch order follow the order in ActuatorValueRepresentation:

	BIT
	BYTE
	WORD
	DOUBLE_WORD
	QUAD_WORD
	SINGLE_FLOAT
	DOUBLE_FLOAT

Representations with no values are simply skipped

*/



#ifndef ARDUINO_ARCH_AVR
static QString valueToString(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep)
{
	QString ret;
	switch(rep) {
	case(BIT):
		ret=QString("BIT(")+ (v.bit?"TRUE":"FALSE")+")";
		break;
	case(BYTE):
		ret="BYTE("+ QString::number(v.byte)+")";
		break;
	case(WORD):
		ret="WORD("+ QString::number(v.word)+")";
		break;
	case(DOUBLE_WORD):
		ret="DWORD("+ QString::number(v.doubleWord)+")";
		break;
	default:// Just make sure its random m-kay?
	case(REPRESENTATION_COUNT):
		ret="OUT_OF_RANGE!";
		break;
	case(QUAD_WORD): {
		ret="QWORD("+ QString::number(v.quadWord)+")";
	}
	break;
	case(SINGLE_FLOAT):
		ret="FLOAT("+ QString::number(v.singlePrecision)+")";
		break;
	case(DOUBLE_FLOAT):
		ret="DOUBLE("+ QString::number(v.doublePrecision)+")";
		break;
	}
	return ret;
}

#endif


bool ArduMYActuatorValueParser::parse(const uint8_t in)
{
	//TODO: Implement a way to explicitly guarantee that set size does not change between calls to parse
	const uint8_t setSize = ( ( nullptr != set ) ? set->size() : 0 );
	const uint8_t enableByteCount = ( ( setSize + 7 ) / 8 );
	switch(step) {
	// We are collecting enable-bits
	case(ENABLED_ACTUATOR_BITS): {
		// Start with no enabled bits in the current byte
		enableBits[byteIndex] = 0x00;
		// Go through input bit by bit
		for(uint8_t i=0; i<8; ++i) {
			// Stop when we reached the actual number of actuators available
			if( ( setSize - enabledActuatorCount ) > 0 ) {
				const uint8_t mask =  ( 1 << i );
				const uint8_t value = ( mask & in );
				// Enable one bit
				if ( 0 != value ) {
					enableBits[ byteIndex ] |= value;
					enabledActuatorCount++;
				}
			} else {
				break;
			}
		}
		// Transition to the next step when all enable bits are gathered
		byteIndex++;
		if( byteIndex == enableByteCount ) {
			//qDebug()<<"PARSER     ACTUATOR COUNT: "<<setSize<<" ENABLE BITS BYTE COUNT: "<<enableByteCount<<" ENABLED COUNT: "<<enabledActuatorCount;
			nextParseStep();
			currentActuatorIndex=-1;
			currentBatchRepresentation=ArduMYActuatorValueRepresentation::BIT;
			nextActuator();
		}
	}
	break;
	// We are collecting values for the enabled actuators
	case(ACTUATOR_VALUE_BATCHES): {
		// TODO: Verify that the case with 0 actuators is handled. See note above.
		switch(currentBatchRepresentation) {
		case(ArduMYActuatorValueRepresentation::BIT): {
			for( uint8_t i = 0; i < 8; ++i ) {
				if(ArduMYActuatorValueRepresentation::BIT == currentBatchRepresentation) {
					ArduMYActuator *actuator = currentActuator();
					if( nullptr != actuator ) {
						Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
						const uint8_t mask = ( 1 << i );
						const uint8_t value= ( in & mask );
						actuator->state.value.bit = ( 0 != value );
						//qDebug()<<"PARSED BIT "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
					}
					nextActuator();
				} else {
					break;
				}
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::BYTE): {
			ArduMYActuator *actuator = currentActuator();
			if(nullptr!=actuator) {
				actuator->state.value.byte=in;
				//qDebug()<<"PARSED BYTE "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
			}
			nextActuator();
		}
		break;
		case(ArduMYActuatorValueRepresentation::WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(2)) {
				ArduMYActuator *actuator = currentActuator();
				Q_CHECK_PTR(actuator);
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.word=converter.uint16[0];
					//qDebug()<<"PARSED WORD "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
				}
				nextActuator();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(4)) {
				ArduMYActuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.doubleWord=converter.uint32[0];
					//qDebug()<<"PARSED DWORD "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
				}
				nextActuator();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(8)) {
				ArduMYActuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.quadWord=converter.uint64;
					//qDebug()<<"PARSED QWORD "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
				}
				nextActuator();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(4)) {
				ArduMYActuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.singlePrecision=converter.float32[0];
					//qDebug()<<"PARSED FLOAT "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
				}
				nextActuator();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(8)) {
				ArduMYActuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.doublePrecision=converter.float64;
					//qDebug()<<"PARSED DOUBLE "<<valueToString(actuator->state.value,actuator->config.representation)<< "@"<<currentActuatorIndex;
				}
				nextActuator();
			}
		}
		break;
		default:
		case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT): {
			// TODO: Handle this as an error somehow
		}
		break;
		}

	}
	break;

	default:
	case(END_OF_OP): {
	}
	break;
	}
	return (END_OF_OP==step);
}

/*
// Look at enabled bits and return the actual servo index by skipping disabled servos
uint8_t ActuatorValueParser::enabledServoByIndex(uint8_t in)
{
	uint8_t out=0;
	uint8_t servosCounted=0;
	for (; out<actuatorPositionsCount; ++out) {
		uint8_t bitIndex=(out%8);
		uint8_t byteIndex=(out+7)/8;
		if( (enableBits[byteIndex] & (1<<bitIndex) ) >0) {
			servosCounted++;
			if(servosCounted==in) {
				break;
			}
		}

	}
	return out;
}
*/
