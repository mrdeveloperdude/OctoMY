#include "ActuatorValueSerializer.hpp"


#include "ArduMY.hpp"


ActuatorValueSerializer::ActuatorValueSerializer()
	: ActuatorValueSerializerBase()
	, actuatorIndex(0)
{

}


// Return true if there is more data remaining
bool ActuatorValueSerializer::hasMoreData() const
{
	return (ActuatorValuesParserStep::END_OF_OP != step);
}

// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
uint8_t ActuatorValueSerializer::nextByte()
{
	uint8_t ret=0x00;
	//TODO: Implement a way to explicitly guarantee that set size does not change between calls to nextByte
	const uint8_t setSize = ( ( nullptr != set ) ? set->size() : 0 );
	switch(step) {
	case(ActuatorValuesParserStep::ENABLED_ACTUATOR_BITS): {
		// Serialize enable-bits
		for(size_t i=0; i<8; ++i) {
			Actuator &a=(*set)[actuatorIndex];
			const uint8_t bit = ( 1 << i );
			if(a.state.isDirty()) {
				ret |= bit;
			}
			actuatorIndex++;
			if(actuatorIndex>=setSize) {
				actuatorIndex=0;
				nextParseStep();
				break;
			}
		}
	}
	break;
	case(ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES): {
		// Serialize actuator values in batches, each batch per one representation type
		switch(currentBatchRepresentation) {
		case(ActuatorValueRepresentation::BIT): {
			// Bit actuator values are accumulated into bytes before sending
			uint8_t bitCount=0;
			for(; actuatorIndex<setSize; ++actuatorIndex) {
				Actuator &a=(*set)[actuatorIndex];
				if(currentBatchRepresentation == a.config.representation) {
					if(a.state.isDirty()) {
						if(a.state.value.bit) {
							ret |= ( 1 << bitCount );
						}
						bitCount++;
						if( bitCount >= 8 ) {
							break;
						}
					}
				}
			}
			if(actuatorIndex>=setSize) {
				nextBatch();
				actuatorIndex=0;
			}
		}
		break;
		/*
		case(ActuatorValueRepresentation::BYTE): {
		ret=config->rangeSpan.byte;
		nextParseStep();
		}
		break;
		case(ActuatorValueRepresentation::WORD): {
		converter.uint16[0]=config->rangeSpan.word;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(2==byteIndex) {
		nextParseStep();
		}
		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_WORD): {
		converter.uint32[0]=config->rangeSpan.doubleWord;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(4==byteIndex) {
		nextParseStep();
		}
		}
		break;
		case(ActuatorValueRepresentation::QUAD_WORD): {
		converter.uint64=config->rangeSpan.quadWord;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(8==byteIndex) {
		nextParseStep();
		}
		}
		break;
		case(ActuatorValueRepresentation::SINGLE_FLOAT): {
		converter.float32[0]=config->rangeSpan.singlePrecision;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(4==byteIndex) {
		nextParseStep();
		}
		}
		break;
		case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
		converter.float64=config->rangeSpan.doublePrecision;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(8==byteIndex) {
		nextParseStep();
		}
		}
		break;
		*/
		default:
		case(ActuatorValueRepresentation::REPRESENTATION_COUNT): {
			//TODO: Handle this as an error somwhow
		}
		break;
		}


		actuatorIndex++;
		if(actuatorIndex>=setSize) {
			actuatorIndex=0;
			nextParseStep();
			break;
		}


	}
	break;
	default:
	case(END_OF_OP): {
		ret=0x00;
	}
	break;
	}

	return ret;
}
