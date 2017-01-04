#include "CommandParser.hpp"

CommandParser::CommandParser()
	: actuators(0)
	, currentCommand(OCTOMY_SYNC)
	, magic{0x0C,0x70,0xFF,0x00}
	, magicDetector(magic,(unsigned char)4)
	, servoConfigParser(*this)
	, servoPosParser(*this)
	, dirtyServos(true)
{

}

// If bad command is detected, go into wait for sync before we try parsing command again,
// and if not, reset states for parsing the command at hand
ParserState CommandParser::prepareCommand(const char in)
{
	ParserState command=(ParserState)in;
	switch(command) {

	// No state to be reset for these commands
	case(OCTOMY_SYNC):
	case(OCTOMY_SET_SERVO_COUNT):
	case(OCTOMY_SET_SERVO_CONFIG):
	case(OCTOMY_SET_SERVO_LIMP):
		break;
	// Reset needed
	case(OCTOMY_SET_SERVO_POSITION): {
		servoPosParser.reset();
	}
	break;

	//All the reasons for abandoning this command and going for a sync instead
	case(OCTOMY_AWAITING_COMMAND):
	case(OCTOMY_UNKNOWN):
	case(OCTOMY_COUNT):
	default: {
		magicDetector.reset();
		command=OCTOMY_SYNC;
	}
	break;
	}
	return command;
}



void CommandParser::parse(const char in)
{
	switch(currentCommand) {
	// Our logic says we need a sync
	case(OCTOMY_SYNC):
	// Some error in command parsing or otherwize says we need a sync
	case(OCTOMY_UNKNOWN):
	default: {
		if(magicDetector.detect((unsigned char)in)) {
			currentCommand=OCTOMY_AWAITING_COMMAND;
		} else {
			// This is where all the bad bytes are skipped
		}
	}
	break;
	// Awaiting command
	case(OCTOMY_AWAITING_COMMAND): {
		currentCommand=prepareCommand(in);
	}
	break;
	// Command: set servo count
	case(OCTOMY_SET_SERVO_COUNT): {
		actuators.setSize((unsigned char)in);
		currentCommand=OCTOMY_AWAITING_COMMAND;
	}
	break;
	// Command: set servo position
	case(OCTOMY_SET_SERVO_POSITION): {
		servoPosParser.parse(in);
		dirtyServos=true;

	}
	break;
	case(OCTOMY_SET_SERVO_LIMP): {
		dirtyServos=true;
	}
	break;


	}

}
