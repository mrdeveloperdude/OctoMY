#include "ArduMYActuatorEnableBits.hpp"

#include "ArduMY.hpp"

ArduMYActuatorEnableBits::ArduMYActuatorEnableBits()
{

}


void ArduMYActuatorEnableBits::serialize()
{
	/*
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
		*/
}

void ArduMYActuatorEnableBits::parse()
{

}
