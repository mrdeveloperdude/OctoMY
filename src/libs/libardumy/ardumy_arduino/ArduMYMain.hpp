#ifndef ARDUMYMAIN_HPP
#define ARDUMYMAIN_HPP

// Only build when we are targeting actual Arduino hardware
#ifdef ARDUINO_ARCH_AVR

#include "ardumy_arduino/ArduMY.hpp"
#include "ardumy_arduino/DynamicArray.hpp"
#include "ardumy_arduino/ArduMYCommandParser.hpp"

#include "ardumy_arduino/BoardInfo.hpp"

#include <Servo.h>

struct ArduMYMain {

	DynamicArray<Servo> servos;

	ArduMYCommandParser &parser;

	ArduMYMain(ArduMYCommandParser &parser)
		: parser(parser)
	{

	}

	void setup();

	void update(uint32_t unow);


};

#endif

#endif // ARDUMYMAIN_HPP
