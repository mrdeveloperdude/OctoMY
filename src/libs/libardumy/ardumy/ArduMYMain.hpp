#ifndef ARDUMYMAIN_HPP
#define ARDUMYMAIN_HPP

#include "ardumy/ArduMY.hpp"
#include "DynamicArray.hpp"
#include "ardumy/ArduMYCommandParser.hpp"

#include "BoardInfo.hpp"

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



#endif // ARDUMYMAIN_HPP
