#include "SetServoPositionsParser.hpp"

#include "CommandParser.hpp"


SetServoPositionsParser::SetServoPositionsParser(CommandParser &commandParser)
	: commandParser(commandParser)
{
	reset();
}

void SetServoPositionsParser::reset()
{
	servoMaxCount=(unsigned char)commandParser.actuators.size();
	servoPositionsCount=0;
	servoPositionIndex=0;
	servoPositionByteIndex=0;
	enableByteCount=(servoMaxCount+7)/8;
	enableByteIndex=0;
}

bool SetServoPositionsParser::parse(const unsigned char in)
{
	// We are collecting enable-bits still
	if(enableByteIndex<enableByteCount) {
		enableBits[enableByteIndex]=in;
		// Count enabled bits
		unsigned char enabledCount=0;
		unsigned char temp=in;
		for(unsigned char i=0; i<8; ++i) {
			enabledCount+=(((temp & 1)>0)?1:0);
			temp>>=1;
		}
		// Accumulate total enabled servos
		servoPositionsCount+=enabledCount;
		enableByteIndex++;
	} else {
		// We are collecting bytes for one position
		if(servoPositionByteIndex<4) {
			converter.uint8[servoPositionByteIndex]=in;
			servoPositionByteIndex++;
		} else {
			// We are collecting positions for one full set
			servoPositionByteIndex=0;
			const float pos=converter.float32[0];
			commandParser.actuators[enabledServoByIndex(servoPositionIndex)].state.value.singlePrecision=pos;
			if(servoPositionIndex<servoPositionsCount) {
				converter.uint8[servoPositionIndex]=in;
				servoPositionIndex++;
			} else {
				return true;
			}
		}
	}
	return false;
}


// Look at enabled bits and return the actual servo index by skipping disabled servos
unsigned char SetServoPositionsParser::enabledServoByIndex(unsigned char in)
{
	unsigned char out=0;
	unsigned char servosCounted=0;
	for (; out<servoPositionsCount; ++out) {
		unsigned char bitIndex=(out%8);
		unsigned char byteIndex=(out+7)/8;
		if( (enableBits[byteIndex] & (1<<bitIndex) ) >0) {
			servosCounted++;
			if(servosCounted==in) {
				break;
			}
		}

	}
	return out;
}
