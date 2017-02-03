#include "CommandSerializer.hpp"

CommandSerializer::CommandSerializer()
	: currentCommand(ParserState::OCTOMY_SYNC)
	, magic{0x0C,0x70,0xFF,0x00}
	, byteIndex(0)
	, actuatorConfigSerializer()
	, actuatorConfigIndex(-1)
	, actuatorValuesSerializer()
{

}


bool CommandSerializer::isReadyForNewCommand()
{
	return (OCTOMY_AWAITING_COMMAND==currentCommand);
}

void CommandSerializer::startSync()
{
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	byteIndex=0;
	currentCommand=OCTOMY_SYNC;
}

void CommandSerializer::startActuatorCount()
{
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	byteIndex=0;
	currentCommand=OCTOMY_SET_ACTUATOR_COUNT;
}


void CommandSerializer::startActuatorConfig(uint8_t index)
{
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	actuatorConfigIndex=index;
	byteIndex=0;
	currentCommand=OCTOMY_SET_ACTUATOR_CONFIG;
}


void CommandSerializer::startActuatorValues()
{
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	byteIndex=0;
	currentCommand=OCTOMY_SET_ACTUATOR_VALUES;
}


// Return true if there is more data remaining to be read
bool CommandSerializer::hasMoreData() const
{
	if(currentCommand==OCTOMY_AWAITING_COMMAND) {
		// 1. Sync due
		if(actuators.isSyncDue()){
			startSync();
		}
		else if(actuators.isCountDirty()){
			startActuatorCount();
		}
		else if(actuators.isConfigDirty()){
			startActuatorConfig(0);
		}
		else if(actuators.isValuesDirty()){
			startActuatorValues();
		}
		// 2. Actuator count changed
		// 3. Actuator config(s) changed
		// 4. Actuator value(s) changed
	}
	return (currentCommand!=OCTOMY_AWAITING_COMMAND);
}

// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
uint8_t CommandSerializer::nextByte()
{
	// TODO: Implement a way to make sure that command does not change mid-serialization
	uint8_t ret=0x00;
	switch(currentCommand) {
	// Rectify unknwon states to mean "awaiting command"
	default:
	case(OCTOMY_UNKNOWN):
		currentCommand=OCTOMY_AWAITING_COMMAND;
		byteIndex=0;
		// TODO: This is just defensive programming, flag this as an error somwhow
		break;
	case(OCTOMY_SYNC): {
		ret=magic[byteIndex];
		byteIndex++;
		if(byteIndex>=sizeof(magic)) {
			byteIndex=0;
			currentCommand=OCTOMY_AWAITING_COMMAND;
		}
	}
	break;
	// Awaiting command
	case(OCTOMY_AWAITING_COMMAND): {
		currentCommand=prepareCommand(in);
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
	return ret;
}
