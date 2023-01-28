#ifndef RCSERVOACTUATOR_HPP
#define RCSERVOACTUATOR_HPP

// Only build when we are targeting actual Arduino hardware
#ifdef OCTOMY_ARDUINO_TARGET

#include "ardumy_arduino/ArduMY.hpp"

#include <Servo.h>

class RCServoActuator
{
	public:
		RCServoActuator();
};

#endif // OCTOMY_ARDUINO_TARGET

#endif // RCSERVOACTUATOR_HPP
