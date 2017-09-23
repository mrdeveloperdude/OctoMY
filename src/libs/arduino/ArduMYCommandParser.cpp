#include "ArduMYCommandParser.hpp"
#include "ArduMYParserState.hpp"


ArduMYCommandParser::ArduMYCommandParser()
	: currentCommand(ArduMYParserState::OCTOMY_SYNC)
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
ArduMYParserState ArduMYCommandParser::prepareCommand(const uint8_t in)
{
	ArduMYParserState command=(ArduMYParserState)in;
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


void ArduMYCommandParser::setup()
{

}

#include "hw/controllers/ardumy/ArduMYTypeConversions.hpp"

void ArduMYCommandParser::parse(const uint8_t in)
{
	//qDebug()<<"PARSED BYTE: "<<in;
	switch(currentCommand) {
	// Our logic says we need a sync
	case(OCTOMY_SYNC):
			//qDebug()<<"GOT SYNC BYTE: "<<in;
	// Some error in command parsing or otherwize says we need a sync
	case(OCTOMY_UNKNOWN):
	default: {
		if(magicDetector.detect(in)) {
			currentCommand=OCTOMY_AWAITING_COMMAND;
		} else {
			// This is where all the bad bytes are skipped
			//qDebug()<<"SKIPPING BAD BYTE: "<<in;
		}
	}
	break;
	// Awaiting command
	case(OCTOMY_AWAITING_COMMAND): {
		//qDebug()<<"GOT NEW COMMAND BYTE: "<<in;
		auto oldCommand=currentCommand;
		currentCommand=prepareCommand(in);
		//qDebug()<<"GOT NEW COMMAND "<<ardumyParserStateToString(oldCommand)<<" ( "<<oldCommand<<" ) -> "<<ardumyParserStateToString(currentCommand)<<" ( "<<currentCommand<<" )";
	}
	break;
	// Status wanted
	case(OCTOMY_GET_STATUS): {
		//qDebug()<<"STATUS PENDING SET";
		sendStatusPending=true;
		currentCommand=OCTOMY_AWAITING_COMMAND;
	}
	break;
	// Command: set actuator count
	case(OCTOMY_SET_ACTUATOR_COUNT): {
		//qDebug()<<"ACTUATOR COUNT SET: "<<in;
		actuators.setSize(in);
		dirtyActuatorValues=true;
		currentCommand=OCTOMY_AWAITING_COMMAND;
	}
	break;

	// Command: set actuator config
	case(OCTOMY_SET_ACTUATOR_CONFIG): {
		if(actuatorConfigIndex < 0) {
			//qDebug()<<"ACTUATOR CONFIG INDEX SET: "<<in;
			actuatorConfigIndex=in;
			if((uint16_t)actuatorConfigIndex>=actuators.size()) {
				qWarning()<<"ERROR: INDEX IS BEYOND ACTUAL ACTUATOR COUNT";
				currentCommand=OCTOMY_SYNC;
				break;
			}
			ArduMYActuator &a=actuators[actuatorConfigIndex];
			actuatorConfigParser.setConfig(a.config);
		} else if(actuatorConfigParser.parse(in)) {
			//qDebug()<<"ACTUATOR CONFIG PARSE FOR INDEX "<<QString::number(actuatorConfigIndex)<<" COMPLETE";
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
