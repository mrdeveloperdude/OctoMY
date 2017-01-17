#include "ActuatorValueParser.hpp"

#include "CommandParser.hpp"


ActuatorValueParser::ActuatorValueParser()
	: ActuatorValueSerializerBase()
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

bool ActuatorValueParser::isDone() const
{
	return (currentBatchRepresentation>=REPRESENTATION_COUNT);
}



// Traverse enabled actuators by batch
void ActuatorValueParser::nextActuator()
{
	byteIndex=0;
	if(nullptr==set) {
		return;
	}
	while ( !isDone() ) {
		currentActuatorIndex++;
		if(currentActuatorIndex >= (int16_t)set->size()) {
			nextBatch();
			currentActuatorIndex=0;
		}
		if( currentActuatorIsEnabled() && currentActuatorIsOfRepresentation(currentBatchRepresentation) ) {
			break;
		}
	}
}


bool ActuatorValueParser::parse(const uint8_t in)
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
			currentBatchRepresentation=ActuatorValueRepresentation::BIT;
			nextParseStep();
			currentActuatorIndex=-1;
			nextActuator(); // NOTE: Will call next batch and eventuall next parse step if no actuators are found
		}
	}
	break;
	// We are collecting values for the enabled actuators
	case(ACTUATOR_VALUE_BATCHES): {
		// TODO: Handle case with 0 actuators
		switch(currentBatchRepresentation) {
		case(ActuatorValueRepresentation::BIT): {
			for( uint8_t i = 0; i < 8; ++i ) {
				if(ActuatorValueRepresentation::BIT == currentBatchRepresentation) {
					Actuator *actuator = currentActuator();
					if( nullptr != actuator ) {
						Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
						const uint8_t mask = ( 1 << i );
						const uint8_t value= ( in & mask );
						actuator->state.value.bit = ( 0 != value );
					}
					nextActuator();
				} else {
					break;
				}
			}
		}
		break;
		case(ActuatorValueRepresentation::BYTE): {
			Actuator *actuator = currentActuator();
			if(nullptr!=actuator) {
				actuator->state.value.byte=in;
			}
			nextActuator();
		}
		break;
		case(ActuatorValueRepresentation::WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(2)) {
				Actuator *actuator = currentActuator();
				Q_CHECK_PTR(actuator);
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.word=converter.uint16[0];
				}
				nextActuator();
			}
		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(4)) {
				Actuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.doubleWord=converter.uint32[0];
				}
				nextActuator();
			}
		}
		break;
		case(ActuatorValueRepresentation::QUAD_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(8)) {
				Actuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.quadWord=converter.uint64;
				}
				nextActuator();
			}
		}
		break;
		case(ActuatorValueRepresentation::SINGLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(4)) {
				Actuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.singlePrecision=converter.float32[0];
				}
				nextActuator();
			}
		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==(8)) {
				Actuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					Q_ASSERT( actuator->config.representation == currentBatchRepresentation );
					actuator->state.value.doublePrecision=converter.float64;
				}
				nextActuator();
				nextParseStep();
			}
		}
		break;
		default:
		case(ActuatorValueRepresentation::REPRESENTATION_COUNT): {
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
