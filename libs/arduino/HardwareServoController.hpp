#ifndef HARDWARESERVOCONTROLLER_HPP
#define HARDWARESERVOCONTROLLER_HPP

#include "DynamicArray.hpp"

#include "ArduMY.hpp"

#include <stdio.h>


struct HardwareServoController {
	DynamicArray<Servo> servos;

	void update(CommandParser &parser);
};



#endif // HARDWARESERVOCONTROLLER_HPP
