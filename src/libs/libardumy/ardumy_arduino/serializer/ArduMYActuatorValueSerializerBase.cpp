#include "ArduMYActuatorValueSerializerBase.hpp"


#include "ardumy_arduino/actuator/ArduMYActuatorSet.hpp"


ArduMYActuatorValueSerializerBase::ArduMYActuatorValueSerializerBase()
	: set(nullptr)
	, step(END_OF_OP)
	, byteIndex(0)
	, currentActuatorIndex(0)
	, currentBatchRepresentation(ArduMYActuatorValueRepresentation::VALREP_BIT)
	, enabledActuatorCount(0)
	, enableBits{0x00}
{

}


void ArduMYActuatorValueSerializerBase::setSet(ArduMYActuatorSet &s)
{
	set=&s;
	reset();
}


void ArduMYActuatorValueSerializerBase::reset()
{
	// There is no we to proceed when set is missing or has 0 size
	if(nullptr==set) {
		step=END_OF_OP;
	} else if(set->size()<=0) {
		step=END_OF_OP;
	} else {
		step=ENABLED_ACTUATOR_BITS;
	}
	byteIndex=0;
	currentActuatorIndex = 0;
	currentBatchRepresentation = ArduMYActuatorValueRepresentation::VALREP_BIT;
	enabledActuatorCount = 0;
	for( size_t i = 0; i < sizeof(enableBits); ++i ) {
		enableBits[i] = 0x00;
	}
}



bool ArduMYActuatorValueSerializerBase::isDone() const
{
	return (currentBatchRepresentation>=VALREP_REPRESENTATION_COUNT);
}



// Traverse enabled actuators by batch
void ArduMYActuatorValueSerializerBase::nextActuator()
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



// Look at flags & type and go from the current step to the next enabled one
void ArduMYActuatorValueSerializerBase::nextParseStep()
{
	switch(step) {
	case(ENABLED_ACTUATOR_BITS): {
		step=ACTUATOR_VALUE_BATCHES;
	}
	break;
	case(ACTUATOR_VALUE_BATCHES): {
		step=END_OF_OP;
	}
	break;
	case(END_OF_OP): {
		// Do nothing. Maybe this is an indication of error, and should be reported?
	}
	break;
	}
	byteIndex=0;
}




ArduMYActuator *ArduMYActuatorValueSerializerBase::currentActuator() const
{
	ArduMYActuator *a=nullptr;
	if( nullptr != set ) {
		a = &(*set)[currentActuatorIndex];
	}
	return a;
}



bool ArduMYActuatorValueSerializerBase::currentActuatorIsEnabled() const
{
	if(nullptr == set) {
		return false;
	}
	if( currentActuatorIndex >= (int16_t)set->size() ) {
		return false;
	}
	const uint8_t byte = currentActuatorIndex / 8;
	const uint8_t bit =  currentActuatorIndex % 8;
	const uint8_t mask = ( 1 << bit );
	const uint8_t value = ( mask & enableBits[byte] );
	return ( 0 != value );
}


bool ArduMYActuatorValueSerializerBase::currentActuatorIsOfRepresentation(ArduMYActuatorValueRepresentation rep) const
{
	if(nullptr==set) {
		return false;
	}
	if(currentActuatorIndex >= (int16_t)set->size()) {
		return false;
	}
	const ArduMYActuatorValueRepresentation currentActuatorRep = (*set)[currentActuatorIndex].config.representation;
	return ( currentActuatorRep == rep );
}


void ArduMYActuatorValueSerializerBase::nextBatch()
{
	switch(currentBatchRepresentation) {
	case(VALREP_BIT):
		currentBatchRepresentation=VALREP_BYTE;
		break;
	case(VALREP_BYTE):
		currentBatchRepresentation=VALREP_WORD;
		break;
	case(VALREP_WORD):
		currentBatchRepresentation=VALREP_DOUBLE_WORD;
		break;
	case(VALREP_DOUBLE_WORD):
		currentBatchRepresentation=VALREP_QUAD_WORD;
		break;
	case(VALREP_QUAD_WORD):
		currentBatchRepresentation=VALREP_SINGLE_FLOAT;
		break;
	case(VALREP_SINGLE_FLOAT):
		currentBatchRepresentation=VALREP_DOUBLE_FLOAT;
		break;
	case(VALREP_DOUBLE_FLOAT):
		currentBatchRepresentation=VALREP_REPRESENTATION_COUNT;
		nextParseStep();
		break;
	//Do nothing at end or error
	case(VALREP_REPRESENTATION_COUNT):
	default:
		break;
	}
}
















