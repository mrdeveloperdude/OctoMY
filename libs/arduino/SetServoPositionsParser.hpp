#ifndef SETSERVOPOSITIONSSTATE_HPP
#define SETSERVOPOSITIONSSTATE_HPP

#include "Converter.hpp"

struct CommandParser;

/*

Temporary storage during parsing of "servo positions" command

 */
struct SetServoPositionsParser {

	CommandParser &commandParser;
	unsigned char servoMaxCount;
	unsigned char servoPositionsCount;
	unsigned char servoPositionIndex;
	unsigned char servoPositionByteIndex;
	unsigned char enableByteCount;
	unsigned char enableByteIndex;
	unsigned char enableBits[32];
	Converter converter;

	SetServoPositionsParser(CommandParser &commandParser);
	void reset();
	bool parse(const unsigned char in);

	// Look at enabled bits and return the actual servo index by skipping disabled servos
	unsigned char enabledServoByIndex(unsigned char in);
};


#endif // SETSERVOPOSITIONSSTATE_HPP
