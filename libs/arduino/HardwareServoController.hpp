#ifndef HARDWARESERVOCONTROLLER_HPP
#define HARDWARESERVOCONTROLLER_HPP

#include "DynamicArray.hpp"

#include <Arduino.h>
#include <Servo.h>

struct CommandParser;


struct HardwareServoController {
	DynamicArray<Servo> servos;

	void update(CommandParser &parser);
};





#endif // HARDWARESERVOCONTROLLER_HPP
