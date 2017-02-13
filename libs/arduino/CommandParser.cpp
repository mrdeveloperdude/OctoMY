#include "CommandParser.hpp"
#include "ParserState.hpp"


CommandParser::CommandParser()
	: currentCommand(ParserState::OCTOMY_SYNC)
	, magic{0x0C,0x70,0xFF,0x00}
	, magicDetector(magic,(uint8_t)4)
	, actuatorConfigParser()
	, actuatorConfigIndex(-1)
	, actuatorValuesParser()
	, dirtyActuatorValues(true)
	, dirtyActuatorConfigs(true)
	, sendStatusPending(false)
{
	actuatorValuesParser.setSet(actuators);
}

// If bad command is detected, go into wait for sync before we try parsing command again,
// and if not, reset states for parsing the command at hand
ParserState CommandParser::prepareCommand(const uint8_t in)
{
	ParserState command=(ParserState)in;
	switch(command) {

	// No state to be reset for these commands
	case(OCTOMY_SYNC):
	case(OCTOMY_SET_ACTUATOR_COUNT):
	case(OCTOMY_SET_ACTUATOR_LIMP):
		break;
	// Reset needed for config
	case(OCTOMY_SET_ACTUATOR_CONFIG): {
		actuatorConfigParser.reset();
	}
	break;

	// Reset needed for values
	case(OCTOMY_SET_ACTUATOR_VALUES): {
		actuatorValuesParser.reset();
	}
	break;

	//All the reasons for abandoning this command and going for a sync instead
	case(OCTOMY_AWAITING_COMMAND):
	case(OCTOMY_GET_STATUS):
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


void CommandParser::setup()
{

}

void CommandParser::parse(const uint8_t in)
{
	switch(currentCommand) {
	// Our logic says we need a sync
	case(OCTOMY_SYNC):
	// Some error in command parsing or otherwize says we need a sync
	case(OCTOMY_UNKNOWN):
	default: {
		if(magicDetector.detect(in)) {
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
	// Status wanted
	case(OCTOMY_GET_STATUS): {
		sendStatusPending=true;
		currentCommand=OCTOMY_AWAITING_COMMAND;
	}
	break;
	// Command: set actuator count
	case(OCTOMY_SET_ACTUATOR_COUNT): {
		actuators.setSize(in);
		dirtyActuatorValues=true;
		currentCommand=OCTOMY_AWAITING_COMMAND;
	}
	break;

	// Command: set actuator config
	case(OCTOMY_SET_ACTUATOR_CONFIG): {
		if(actuatorConfigIndex < 0) {
			actuatorConfigIndex=in;
			if((uint16_t)actuatorConfigIndex>=actuators.size()) {
				//TODO: Handle error somehow. Maybe ask for sync?
			}
			ArduMYActuator &a=actuators[actuatorConfigIndex];
			actuatorConfigParser.setConfig(a.config);
		} else if(actuatorConfigParser.parse(in)) {
			dirtyActuatorConfigs=true;
			actuatorConfigIndex=-1;
			currentCommand=OCTOMY_AWAITING_COMMAND;
		}
	}
	break;
	// Command: set actuator values
	case(OCTOMY_SET_ACTUATOR_VALUES): {
		if(actuatorValuesParser.parse(in)) {
			dirtyActuatorValues=true;
			currentCommand=OCTOMY_AWAITING_COMMAND;
		}
	}
	break;
	case(OCTOMY_SET_ACTUATOR_LIMP): {
		dirtyActuatorValues=true;
	}
	break;


	}

}
