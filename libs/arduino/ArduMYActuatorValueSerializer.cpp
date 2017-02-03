#include "ArduMYActuatorValueSerializer.hpp"


#include "ArduMY.hpp"


ArduMYActuatorValueSerializer::ArduMYActuatorValueSerializer()
	: ArduMYActuatorValueSerializerBase()
	, representationByteCount(0)
	, enableByteIndex(0)
{

}

/*
void ActuatorValueSerializer::nextActuator()
{
	if(nullptr==set) {
		return;
	}
	currentActuatorIndex++;
	byteIndex=0;
	if(currentActuatorIndex>=set->size()) {
		nextBatch();
	}
}

*/


void ArduMYActuatorValueSerializer::reset(){
	ArduMYActuatorValueSerializerBase::reset();
	enableByteIndex=0;
}
// Return true if there is more data remaining
bool ArduMYActuatorValueSerializer::hasMoreData() const
{
	return (ActuatorValuesParserStep::END_OF_OP != step);
}


// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
uint8_t ArduMYActuatorValueSerializer::nextByte()
{
	uint8_t ret=0x00;
	//TODO: Implement a way to explicitly guarantee that set size does not change between calls to nextByte
	const uint8_t setSize = ( ( nullptr != set ) ? set->size() : 0 );
	switch(step) {
	case(ActuatorValuesParserStep::ENABLED_ACTUATOR_BITS): {
		// Serialize enable-bits
		for(uint8_t bit=0; bit<8; ++bit) {
			if(currentActuatorIndex<setSize) {
				ArduMYActuator *actuator=currentActuator();
				if(nullptr!=actuator) {
					ArduMYActuator &a=*actuator;
					const uint8_t mask = ( 1 << bit );
					if(a.state.isDirty()) {
						enabledActuatorCount++;
						ret |= mask;
					}
					currentActuatorIndex++;
				}
			} else {
				currentActuatorIndex=0;
				nextParseStep();
				if(enabledActuatorCount<=0) { // No enable bits means no values will be transferred, means we end the show here.
					nextParseStep();
				}
				break;
			}
		}
		enableBits[enableByteIndex]=ret;
		enableByteIndex++;
	}
	break;
	case(ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES): {
		// Serialize actuator values in batches, each batch per one representation type
		for(; currentBatchRepresentation<ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT; nextBatch() ) {
			//representationByteCount
			uint8_t bitCount=0;
			for(; currentActuatorIndex<setSize; ++currentActuatorIndex) {
				ArduMYActuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					const ArduMYActuator &a=*actuator;
					if(a.config.representation == currentBatchRepresentation && a.state.isDirty()) {
						switch (currentBatchRepresentation) {
						case (ArduMYActuatorValueRepresentation::BIT): {
							if(a.state.value.bit) {
								ret |= ( 1 << bitCount );
							}
							bitCount++;
							//qDebug()<<"Serialized BIT value of "<<a.state.value.bit<<" at bit "<<bitCount<<" of byte "<<byteIndex<<" for actuator "<<currentActuatorIndex;
							nextActuator();
							if( bitCount >= 8 ) {
								bitCount=0;
								goto byte_ready;
							}
						}
						break;
						case (ArduMYActuatorValueRepresentation::BYTE): {
							ret=a.state.value.byte;
							nextActuator();
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::WORD): {
							converter.uint64=0;
							converter.uint16[0]=a.state.value.word;
							ret=converter.uint8[byteIndex];
							byteIndex++;
							if(2==byteIndex) {
								nextActuator();
							}
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
							converter.uint64=0;
							converter.uint32[0]=a.state.value.doubleWord;
							ret=converter.uint8[byteIndex];
							byteIndex++;
							if(4==byteIndex) {
								nextActuator();
							}
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::QUAD_WORD): {
							converter.uint64=a.state.value.quadWord;
							ret=converter.uint8[byteIndex];
							byteIndex++;
							if(8==byteIndex) {
								nextActuator();
							}
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
							converter.uint64=0;
							converter.float32[0]=a.state.value.singlePrecision;
							ret=converter.uint8[byteIndex];
							byteIndex++;
							if(4==byteIndex) {
								nextActuator();
							}
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
							converter.float64=a.state.value.doublePrecision;
							ret=converter.uint8[byteIndex];
							byteIndex++;
							if(8==byteIndex) {
								nextActuator();
							}
							goto byte_ready;
						}
						break;
						default:
							break;
						}
					}
				}
			}
			currentActuatorIndex=0;
			// Handle left-over bit values
			if( bitCount >0 ) {
				bitCount=0;
				goto byte_ready;
			}
		}
		nextParseStep();
	}
	break;
	default:
	case(END_OF_OP): {
		ret=0x00;
	}
	break;
	}

byte_ready:
	return ret;
}







/*
if(ActuatorValueRepresentation::BIT == currentBatchRepresentation) {
// Bit actuator values are accumulated into bytes before sending
uint8_t bitCount=0;
Actuator &a=(*set)[currentActuatorIndex];
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
Actuator &a=(*set)[currentActuatorIndex];
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
Actuator &a=(*set)[currentActuatorIndex];
if(currentBatchRepresentation == a.config.representation) {
if(a.state.isDirty()) {
	converter.uint16[0]=a.state.value.word;
	ret=converter.uint8[byteIndex];
	byteIndex++;
	if(2==byteIndex) {
		currentActuatorIndex++;
	}
	break;
} else {
	currentActuatorIndex++;
}
} else {
currentActuatorIndex++;
}
}


if(ActuatorValueRepresentation::DOUBLE_WORD == currentBatchRepresentation) {

for(; currentActuatorIndex<setSize;) {
Actuator &a=(*set)[currentActuatorIndex];
if(currentBatchRepresentation == a.config.representation) {
	if(a.state.isDirty()) {
		converter.uint32[0]=a.state.value.doubleWord;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(4==byteIndex) {
			currentActuatorIndex++;
		}
		break;
	} else {
		currentActuatorIndex++;
	}
} else {
	currentActuatorIndex++;
}
}
}

if(ActuatorValueRepresentation::QUAD_WORD == currentBatchRepresentation) {
for(; currentActuatorIndex<setSize;) {
Actuator &a=(*set)[currentActuatorIndex];
if(currentBatchRepresentation == a.config.representation) {
	if(a.state.isDirty()) {
		converter.uint64=a.state.value.quadWord;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(8==byteIndex) {
			currentActuatorIndex++;
		}
		break;
	} else {
		currentActuatorIndex++;
	}
} else {
	currentActuatorIndex++;
}
}
}

if(ActuatorValueRepresentation::SINGLE_FLOAT == currentBatchRepresentation) {
for(; currentActuatorIndex<setSize;) {
Actuator &a=(*set)[currentActuatorIndex];
if(currentBatchRepresentation == a.config.representation) {
	if(a.state.isDirty()) {
		converter.float32[0]=a.state.value.singlePrecision;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(4==byteIndex) {
			currentActuatorIndex++;
		}
		break;
	} else {
		currentActuatorIndex++;
	}
} else {
	currentActuatorIndex++;
}
}
}

if(ActuatorValueRepresentation::DOUBLE_FLOAT == currentBatchRepresentation) {

for(; currentActuatorIndex<setSize;) {
Actuator &a=(*set)[currentActuatorIndex];
if(currentBatchRepresentation == a.config.representation) {
	if(a.state.isDirty()) {
		converter.float64=a.state.value.doublePrecision;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(8==byteIndex) {
			currentActuatorIndex++;
		}
		break;
	} else {
		currentActuatorIndex++;
	}
} else {
	currentActuatorIndex++;
}
}
}

if(currentActuatorIndex>=setSize) {

nextBatch();
currentActuatorIndex=0;
//if(ActuatorValueRepresentation::REPRESENTATION_COUNT == currentBatchRepresentation) {				nextParseStep();			}
}
*/
