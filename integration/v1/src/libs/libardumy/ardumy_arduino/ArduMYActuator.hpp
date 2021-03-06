#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include "ardumy_arduino/ArduMY.hpp"

/*

  Actuator is constructed from its config and its state.

  The config are all the static parameters the define its structure and features, while state is the actual dynamic value of the actuator at run time.

  The config is meant to be changed seldomly, while the state will change continuously during opperation. The serial protocol will thus optimize for quick sending of state while config is less optimized.

  Given that it will run on limited embedded hardware, the code standard of ArduMY™ will deviate from that of OctoMY™. The actuator source code is shared between ArduMY™ and OctoMY™ codebase, and thus will not rely on any Qt types or code. Both ArduMY™ & OctoMY™ may wrap this code to add convenience each in their own way.


*/

#include "ardumy_arduino/ArduMYActuatorValueRepresentation.hpp"


#include "ardumy_arduino/ArduMYActuatorType.hpp"
#include "ardumy_arduino/ArduMYActuatorValue.hpp"

#include "ardumy_arduino/ArduMYActuatorFlags.hpp"

#include "ardumy_arduino/ArduMYActuatorConfig.hpp"
#include "ardumy_arduino/ArduMYActuatorState.hpp"

struct ArduMYActuator {
	ArduMYActuatorConfig config;
	ArduMYActuatorState state;

	bool isEqual(const ArduMYActuator &other) const
	{
		return config.isEqual(other.config) && state.isEqual(other.state, config.representation);
	}

	float valueFloat(){
		return state.value.toFloat(config.representation);
	}
};


bool operator== (const ArduMYActuator &a, const ArduMYActuator &b);
bool operator!= (const ArduMYActuator &a, const ArduMYActuator &b);



#endif // ACTUATOR_HPP
