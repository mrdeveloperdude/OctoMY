#include "ActuatorValueSerializerBase.hpp"



ActuatorValueSerializerBase::ActuatorValueSerializerBase()
	: set(nullptr)
	, step(END_OF_OP)
	, byteIndex(0)
{

}


void ActuatorValueSerializerBase::setSet(ActuatorSet &s)
{
	set=&s;
	reset();
}


void ActuatorValueSerializerBase::reset()
{
	step=ENABLED_ACTUATOR_BITS;
	byteIndex=0;
}



// Look at flags & type and go from the current step to the next enabled one
void ActuatorValueSerializerBase::nextParseStep()
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
