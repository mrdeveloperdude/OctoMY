#include "ardumy/ArduMYActuatorValueSerializer.hpp"


#include "ardumy/ArduMY.hpp"

//#include "hw/controllers/ardumy/ArdumyTypeConversions.hpp"

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


void ArduMYActuatorValueSerializer::reset()
{
	ArduMYActuatorValueSerializerBase::reset();
	enableByteIndex=0;
}
// Return true if there is more data remaining
bool ArduMYActuatorValueSerializer::hasMoreData() const
{
	return (ArduMYActuatorValuesParserStep::END_OF_OP != step);
}


// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
uint8_t ArduMYActuatorValueSerializer::nextByte()
{
	uint8_t ret=0x00;
	//TODO: Implement a way to explicitly guarantee that set size does not change between calls to nextByte
	const uint8_t setSize = ( ( nullptr != set ) ? set->size() : 0 );
	const ArduMYActuatorValuesParserStep lastStep=step;
	switch(step) {
	case(ArduMYActuatorValuesParserStep::ENABLED_ACTUATOR_BITS): {
		// Serialize enable-bits
		for(uint8_t bit=0; bit<8; ++bit) {
			if(currentActuatorIndex<setSize) {
				ArduMYActuator *actuator=currentActuator();
				if(nullptr!=actuator) {
					ArduMYActuator &a=*actuator;
					if(a.state.isDirty()) {
						enabledActuatorCount++;
						const uint8_t mask = ( 1 << bit );
						ret |= mask;
					}
					currentActuatorIndex++;
				}
			}
			if(currentActuatorIndex==setSize) {
				currentActuatorIndex=0;
				nextParseStep();
				if(enabledActuatorCount<=0) { // No enable bits means no values will be transferred, means we end the show here.
					nextParseStep();
				}
				break;
			} else if(currentActuatorIndex>setSize) {
				//TODO: Handle this as an error condition!
			}
		}
		enableBits[enableByteIndex]=ret;
		enableByteIndex++;
	}
	break;
	case(ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES): {
		// Serialize actuator values in batches, each batch per one representation type
		for(; currentBatchRepresentation<ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT; nextBatch() ) {
			//representationByteCount
			uint8_t bitCount=0;
			for(; currentActuatorIndex<setSize; ++currentActuatorIndex) {
				//qDebug()<<"LOLS";
				ArduMYActuator *actuator = currentActuator();
				if(nullptr!=actuator) {
					const ArduMYActuator &a=*actuator;
					if(a.config.representation == currentBatchRepresentation && a.state.isDirty()) {
						switch (currentBatchRepresentation) {
						case (ArduMYActuatorValueRepresentation::VALREP_BIT): {
							while(ArduMYActuatorValueRepresentation::VALREP_BIT==currentBatchRepresentation) {
								bool wasDirty=false;
								actuator = currentActuator();
								if(nullptr!=actuator) {
									const ArduMYActuator &a2=*actuator;
									wasDirty=a2.state.isDirty();
									if(wasDirty) {
										if(a2.state.value.bit) {
											ret |= ( 1 << bitCount );
										}
										//qDebug()<<"Serialized BIT value of "<<(a.state.value.bit)<<" at bit "<<bitCount<<" of byte "<<byteIndex<<" for actuator "<<currentActuatorIndex;
										//qDebug()<<"Serialized ACTUATOR: "<< ardumyActuatorToString(a2);
										//"value of "<<ardumyActuatorValueToString(a.state.value,a.config.representation)<<" at bit "<<bitCount<<" of byte "<<byteIndex<<" for actuator "<<currentActuatorIndex;
										bitCount++;
									} else {
										//qDebug()<<"NOT DIRTY: "<<currentActuatorIndex;
									}
								}
								nextActuator();
								if(wasDirty) {
									if( bitCount >= 8 ) {
										bitCount=0;
										goto byte_ready;
									}
								}
							}
						}
						break;
						case (ArduMYActuatorValueRepresentation::VALREP_BYTE): {
							ret=a.state.value.byte;
							nextActuator();
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::VALREP_WORD): {
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
						case (ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
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
						case (ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
							converter.uint64=a.state.value.quadWord;
							ret=converter.uint8[byteIndex];
							byteIndex++;
							if(8==byteIndex) {
								nextActuator();
							}
							goto byte_ready;
						}
						break;
						case (ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
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
						case (ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
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
	// Detect completion:
	if( (lastStep!= step) && (END_OF_OP == step) && (nullptr!=set) ) {
		// At this point we are no longer dirty, pop the Champagne!
		set->setValueDirty(false);
	}
	return ret;
}
