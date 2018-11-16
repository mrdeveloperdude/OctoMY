#ifndef RCSERVOACTUATOR_HPP
#define RCSERVOACTUATOR_HPP

// Only build when we are targeting actual Arduino hardware
#ifdef ARDUINO_ARCH_AVR

#include "ardumy_arduino/ArduMY.hpp"

#include <Servo.h>

class RCServoActuator
{
	public:
		RCServoActuator();
};

#endif // ARDUINO_ARCH_AVR

#endif // RCSERVOACTUATOR_HPP
