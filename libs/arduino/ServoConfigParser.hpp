#ifndef SERVOCONMFIGPARSER_HPP
#define SERVOCONMFIGPARSER_HPP
#include "ArduMY.hpp"
#include "Converter.hpp"

#include "Actuator.hpp"


struct CommandParser;

/*

Temporary storage during parsing of "servo positions" command

 */

struct ServoConfigParser {

	enum ServoConfigParseStep: uint8_t {

		ACTUATOR_INDEX, FLAGS, TYPE, REPRESENTATION, NICK, GEAR_NUMERATOR, GEAR_DENOMINATOR, POSITION_FEEDBACK, TACHOMETER, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_DEBOUNCE, LIMIT_SWITCH_PIN_START, LIMIT_SWITCH_PIN_END, LIMIT_SWITCH_DEBOUNCE, STEP_PHASE_COUNT, STEP_STEPS_PER_ROTATION, RC_SERVO_PIN, RANGE_START, RANGE_SPAN, END_OF_PARSE

	};

	ServoConfigParseStep step;

	uint8_t actuatorIndex;
	uint16_t byteIndex;

	CommandParser &commandParser;
	Converter converter;

	ServoConfigParser(CommandParser &commandParser);

	ActuatorConfig &currentConfig();

	void nextStep();
	void reset();
	bool parse(const uint8_t in);

};


#endif // SERVOCONMFIGPARSER_HPP
