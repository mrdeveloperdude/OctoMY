#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include "ArduMY.hpp"

/*

  Actuator is constructed from its config and its state.

  The config are all the static parameters the define its structure and features, while state is the actual dynamic value of the actuator at run time.

  The config is meant to be changed seldomly, while the state will change continuously during opperation. The serial protocol will thus optimize for quick sending of state while config is less optimized.

  Given that it will run on limited embedded hardware, the code standard of ArduMY™ will deviate from that of OctoMY™. The actuator source code is shared between ArduMY™ and OctoMY™ codebase, and thus will not rely on any Qt types or code. Both ArduMY™ & OctoMY™ may wrap this code to add convenience each in their own way.


*/

#include "ActuatorValueRepresentation.hpp"


#include "ActuatorType.hpp"
#include "ActuatorValue.hpp"

#include "ActuatorFlags.hpp"

#include "ActuatorConfig.hpp"
#include "ActuatorState.hpp"

struct Actuator {
	ActuatorConfig config;
	ActuatorState state;

	bool isEqual(const Actuator &other) const
	{
		return config.isEqual(other.config) && state.isEqual(other.state, config.representation);
	}

};


bool operator== (const Actuator &a, const Actuator &b);
bool operator!= (const Actuator &a, const Actuator &b);



#endif // ACTUATOR_HPP
