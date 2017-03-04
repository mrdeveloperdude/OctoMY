#include "ArduMYCommandSerializer.hpp"


ArduMYCommandSerializer::ArduMYCommandSerializer(ArduMYActuatorSet &actuators)
	: actuators(actuators)
	, currentCommand(ArduMYParserState::OCTOMY_SYNC)
	, magic{0x0C,0x70,0xFF,0x00}
	, byteIndex(0)
	, actuatorConfigSerializer()
	, actuatorConfigIndex(-1)
	, actuatorValuesSerializer()
{

}


bool ArduMYCommandSerializer::isReadyForNewCommand()
{
	return (OCTOMY_AWAITING_COMMAND==currentCommand);
}

void ArduMYCommandSerializer::startSync()
{
	qDebug()<<"ARDUMY COMMAND SERIALIZER START SYNC";
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	byteIndex=0;
	currentCommand=OCTOMY_SYNC;
}

void ArduMYCommandSerializer::startActuatorCount()
{
	qDebug()<<"ARDUMY COMMAND SERIALIZER COUNT";
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	byteIndex=0;
	currentCommand=OCTOMY_SET_ACTUATOR_COUNT;
}


void ArduMYCommandSerializer::startActuatorConfig(uint8_t index)
{
	qDebug()<<"ARDUMY COMMAND SERIALIZER ACTUATOR CONFIG FOR INDEX "<<index;
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	actuatorConfigIndex=index;
	byteIndex=0;
	actuatorConfigSerializer.reset();
	currentCommand=OCTOMY_SET_ACTUATOR_CONFIG;
}


void ArduMYCommandSerializer::startActuatorValues()
{
	qDebug()<<"ARDUMY COMMAND SERIALIZER ACTUATOR VALUES";
	// Don't interrupt current session
	if(isReadyForNewCommand()) {
		return;
	}
	byteIndex=0;
	actuatorValuesSerializer.reset();
	currentCommand=OCTOMY_SET_ACTUATOR_VALUES;
}


// Return true if there is more data remaining to be read
bool ArduMYCommandSerializer::hasMoreData()
{
	if(OCTOMY_AWAITING_COMMAND==currentCommand) {
		// 1. Sync due
		if(actuators.isSyncDue()) {
			startSync();
			actuators.setSyncDue(false);
		}
		// 2. Actuator count changed
		else if(actuators.isCountDirty()) {
			startActuatorCount();
			actuators.setCountDirty(false);
		}
		// 3. Actuator config(s) changed
		else if(actuators.isConfigDirty()) {
			uint32_t sz=actuators.size();
			bool any=false;
			for(uint8_t id=0; id<sz; ++id) {
				ArduMYActuator &act=actuators[id];
				if(act.config.isDirty()) {
					startActuatorConfig(id);
					any=true;
					break;
				}
			}
			if(!any) {
				actuators.setConfigDirty(false);
				return hasMoreData();
			}
		}
		// 4. Actuator value(s) changed
		else if(actuators.isValuesDirty()) {
			startActuatorValues();
			actuators.setValuesDirty(false);
		}
	}
	return (currentCommand!=OCTOMY_AWAITING_COMMAND);
}

// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
uint8_t ArduMYCommandSerializer::nextByte()
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
		// NOTE: We should not be called when this is the command, so might consider putting up a warning in this case
	}
	break;
	// Command: set actuator count
	case(OCTOMY_SET_ACTUATOR_COUNT): {
		uint32_t sz=actuators.size();
		if(sz>0xFF) {
			//NOTE: This is DEFENSIVE/ERROR RECOVERY, it should never happen
			sz=0xFF;
		}
		ret=(uint8_t)sz;
		currentCommand=OCTOMY_AWAITING_COMMAND;
	}
	break;

	// Command: set actuator config
	case(OCTOMY_SET_ACTUATOR_CONFIG): {
		if(actuatorConfigSerializer.hasMoreData()) {
			ret=actuatorConfigSerializer.nextByte();
		} else {
			// ERROR! This should never happen
		}
		if(!actuatorConfigSerializer.hasMoreData()) {
			currentCommand=OCTOMY_AWAITING_COMMAND;
		}
	}
	break;
	// Command: set actuator values
	case(OCTOMY_SET_ACTUATOR_VALUES): {
		if(actuatorValuesSerializer.hasMoreData()) {
			ret=actuatorValuesSerializer.nextByte();
		} else {
			// ERROR! This should never happen
		}
		if(!actuatorValuesSerializer.hasMoreData()) {
			currentCommand=OCTOMY_AWAITING_COMMAND;
		}
	}
	break;
	case(OCTOMY_SET_ACTUATOR_LIMP): {
	}
	break;
	}
	return ret;
}
