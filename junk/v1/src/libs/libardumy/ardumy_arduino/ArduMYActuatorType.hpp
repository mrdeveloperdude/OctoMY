#ifndef ARDUMYACTUATORTYPE_HPP
#define ARDUMYACTUATORTYPE_HPP

#include "ardumy_arduino/ArduMY.hpp"

enum ArduMYActuatorType: uint8_t {
	DC_MOTOR, STEP_MOTOR, RC_SERVO, RELAY, TYPE_COUNT
};


#endif // ARDUMYACTUATORTYPE_HPP
