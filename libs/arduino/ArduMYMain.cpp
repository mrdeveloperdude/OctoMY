#include "ArduMYMain.hpp"

#include "CommandParser.hpp"

void ArduMYMain::setup()
{

}

void ArduMYMain::update(uint32_t unow)
{
	if(parser.sendStatusPending) {
		BoardInfo bi;
		Serial.println("HERE IS YOUR FUCKING STATUS SCHWINHUND");
		parser.sendStatusPending=false;
	}
	auto hwServoCount=servos.size();
	auto parserServoCount=parser.actuators.size();
	if(hwServoCount!=parserServoCount) {
		// Disengage all servos
		for(unsigned int i=0; i<hwServoCount; ++i) {
			servos[i].detach();
		}
		// Reallocate
		servos.setSize(parserServoCount);
		// Engage all servos
		for(unsigned int i=0; i<parserServoCount; ++i) {
			servos[i].attach(parser.actuators[i].config.rcServoPin);
		}
	} else if(parser.dirtyActuatorValues) {
		// Disengage only dirty servos
		for(unsigned int i=0; i<hwServoCount; ++i) {
			if(parser.actuators[i].config.isDirty()) {
				servos[i].detach();
			}
		}
		// Engage only dirty servos
		for(unsigned int i=0; i<parserServoCount; ++i) {
			if(parser.actuators[i].config.isDirty()) {
				parser.actuators[i].config.setDirty(false);
				servos[i].attach(parser.actuators[i].config.rcServoPin);
			}
		}
		parser.dirtyActuatorValues=false;
	}
}

