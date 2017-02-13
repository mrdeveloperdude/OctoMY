#ifndef ARDUMYMAIN_HPP
#define ARDUMYMAIN_HPP

#include "ArduMY.hpp"
#include "DynamicArray.hpp"
#include "CommandParser.hpp"

#include "BoardInfo.hpp"

#include <Servo.h>

struct ArduMYMain {

	DynamicArray<Servo> servos;

	CommandParser &parser;

	ArduMYMain(CommandParser &parser)
		: parser(parser)
	{

	}

	void setup();

	void update(uint32_t unow);


};



#endif // ARDUMYMAIN_HPP
