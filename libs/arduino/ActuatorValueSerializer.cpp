#include "ActuatorValueSerializer.hpp"


#include "ArduMY.hpp"


ActuatorValueSerializer::ActuatorValueSerializer()
	: ActuatorValueSerializerBase()
	, actuatorIndex(0)
{

}

void ActuatorValueSerializer::nextActuator()
{
	if(nullptr==set) {
		return;
	}
	currentActuatorIndex++;
	if(currentActuatorIndex>=set->size()) {
		nextBatch();
	}
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
				enabledActuatorCount++;
				ret |= bit;
			}
			actuatorIndex++;
			if(actuatorIndex>=setSize) {
				actuatorIndex=0;
				nextParseStep();
				if(enabledActuatorCount<=0) { // No enable bits means no values will be transferred, means we end the show here.
					nextParseStep();
				}
				break;
			}
		}
	}
	break;
	case(ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES): {
		// Serialize actuator values in batches, each batch per one representation type

		if(ActuatorValueRepresentation::BIT == currentBatchRepresentation) {
			// Bit actuator values are accumulated into bytes before sending
			uint8_t bitCount=0;
			Actuator &a=(*set)[actuatorIndex];
			if(currentBatchRepresentation == a.config.representation) {
				if(a.state.isDirty()) {
					if(a.state.value.bit) {
						ret |= ( 1 << bitCount );
					}
					bitCount++;
					if( bitCount >= 8 ) {
						nextActuator();
					}
				}
			}
		}


		if(ActuatorValueRepresentation::BYTE == currentBatchRepresentation) {
			while(ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES==step) {
				Actuator &a=(*set)[actuatorIndex];
				nextActuator();
				if(currentBatchRepresentation == a.config.representation) {
					if(a.state.isDirty()) {
						ret=a.state.value.byte;
						break;
					}
				}
			}
		}

		if(ActuatorValueRepresentation::WORD == currentBatchRepresentation) {
			Actuator &a=(*set)[actuatorIndex];
			if(currentBatchRepresentation == a.config.representation) {
				if(a.state.isDirty()) {
					converter.uint16[0]=a.state.value.word;
					ret=converter.uint8[byteIndex];
					byteIndex++;
					if(2==byteIndex) {
						actuatorIndex++;
					}
					break;
				} else {
					actuatorIndex++;
				}
			} else {
				actuatorIndex++;
			}
		}


		if(ActuatorValueRepresentation::DOUBLE_WORD == currentBatchRepresentation) {

			for(; actuatorIndex<setSize;) {
				Actuator &a=(*set)[actuatorIndex];
				if(currentBatchRepresentation == a.config.representation) {
					if(a.state.isDirty()) {
						converter.uint32[0]=a.state.value.doubleWord;
						ret=converter.uint8[byteIndex];
						byteIndex++;
						if(4==byteIndex) {
							actuatorIndex++;
						}
						break;
					} else {
						actuatorIndex++;
					}
				} else {
					actuatorIndex++;
				}
			}
		}

		if(ActuatorValueRepresentation::QUAD_WORD == currentBatchRepresentation) {
			for(; actuatorIndex<setSize;) {
				Actuator &a=(*set)[actuatorIndex];
				if(currentBatchRepresentation == a.config.representation) {
					if(a.state.isDirty()) {
						converter.uint64=a.state.value.quadWord;
						ret=converter.uint8[byteIndex];
						byteIndex++;
						if(8==byteIndex) {
							actuatorIndex++;
						}
						break;
					} else {
						actuatorIndex++;
					}
				} else {
					actuatorIndex++;
				}
			}
		}

		if(ActuatorValueRepresentation::SINGLE_FLOAT == currentBatchRepresentation) {
			for(; actuatorIndex<setSize;) {
				Actuator &a=(*set)[actuatorIndex];
				if(currentBatchRepresentation == a.config.representation) {
					if(a.state.isDirty()) {
						converter.float32[0]=a.state.value.singlePrecision;
						ret=converter.uint8[byteIndex];
						byteIndex++;
						if(4==byteIndex) {
							actuatorIndex++;
						}
						break;
					} else {
						actuatorIndex++;
					}
				} else {
					actuatorIndex++;
				}
			}
		}

		if(ActuatorValueRepresentation::DOUBLE_FLOAT == currentBatchRepresentation) {

			for(; actuatorIndex<setSize;) {
				Actuator &a=(*set)[actuatorIndex];
				if(currentBatchRepresentation == a.config.representation) {
					if(a.state.isDirty()) {
						converter.float64=a.state.value.doublePrecision;
						ret=converter.uint8[byteIndex];
						byteIndex++;
						if(8==byteIndex) {
							actuatorIndex++;
						}
						break;
					} else {
						actuatorIndex++;
					}
				} else {
					actuatorIndex++;
				}
			}
		}

		if(actuatorIndex>=setSize) {

			nextBatch();
			actuatorIndex=0;
			if(ActuatorValueRepresentation::REPRESENTATION_COUNT == currentBatchRepresentation) {
				nextParseStep();
			}
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
