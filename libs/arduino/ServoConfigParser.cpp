#include "ServoConfigParser.hpp"

#include "CommandParser.hpp"


ServoConfigParser::ServoConfigParser(CommandParser &commandParser)
	: commandParser(commandParser)
	, step(ACTUATOR_INDEX)
	, byteIndex(0)
	, actuatorIndex(0)
{
	reset();
}

void ServoConfigParser::reset()
{
	step=ACTUATOR_INDEX;
	byteIndex=0;
	actuatorIndex=0;
}


ActuatorConfig &ServoConfigParser::currentConfig()
{
	return commandParser.actuators[actuatorIndex].config;
}

// Look at flags and go from the current step to the next enabled one
void ServoConfigParser::nextStep()
{
	ActuatorConfig &act=currentConfig();
	switch(step) {
	case(ACTUATOR_INDEX): {
		step=FLAGS;
	}
	break;
	case(FLAGS): {
		step=TYPE;
	}
	break;
	case(TYPE): {
		step=REPRESENTATION;
	}
	break;
	case(REPRESENTATION): {
		step=NICK;
	}
	break;
	case(NICK): {
		if(act.hasGearRatio()) {
			step=GEAR_NUMERATOR;
		} else {
			step=GEAR_DENOMINATOR;
			nextStep();
		}
	}
	break;
	case(GEAR_NUMERATOR): {
		step=GEAR_DENOMINATOR;
	}
	break;
	case(GEAR_DENOMINATOR): {
		step=POSITION_FEEDBACK;
		if(!act.hasPositionFeedback()) {
			nextStep();
		}
	}
	break;
	case(POSITION_FEEDBACK): {
		step=TACHOMETER;
		if(!act.hasTachometer()) {
			nextStep();
		}
	}
	break;
	case(TACHOMETER): {
		if(act.hasIncrementalEncoder()) {
			step=ENCODER_PIN_A;
		} else {
			step=ENCODER_DEBOUNCE;
			nextStep();
		}
	}
	break;
	case(ENCODER_PIN_A): {
		step=ENCODER_PIN_B;
	}
	break;
	case(ENCODER_PIN_B): {
		step=ENCODER_DEBOUNCE;
	}
	break;
	case(ENCODER_DEBOUNCE): {
		step=LIMIT_SWITCH_PIN_START;
		if(!act.hasLimitSwitchStart()) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_PIN_START): {
		step=LIMIT_SWITCH_PIN_END;
		if(!act.hasLimitSwitchEnd()) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_PIN_END): {
		step=LIMIT_SWITCH_DEBOUNCE;
		if((!act.hasLimitSwitchStart()) && (!act.hasLimitSwitchEnd() )) {
			nextStep();
		}
	}
	break;
	case(LIMIT_SWITCH_DEBOUNCE): {
		if(STEP_MOTOR==act.type) {
			step=STEP_PHASE_COUNT;
		} else {
			step=STEP_STEPS_PER_ROTATION;
			nextStep();
		}
	}
	break;
	case(STEP_PHASE_COUNT): {
		step=STEP_STEPS_PER_ROTATION;
	}
	break;
	case(STEP_STEPS_PER_ROTATION): {
		if(RC_SERVO==act.type) {
			step=RC_SERVO_PIN;
		} else {
			step=RANGE_START;
		}
	}
	break;
	case(RC_SERVO_PIN): {
		step=RANGE_START;
	}
	break;
	case(RANGE_START): {
		step=RANGE_SPAN;
	}
	break;
	case(RANGE_SPAN): {
		step=END_OF_PARSE;
	}
	break;
	case(END_OF_PARSE): {
		// Do nothing. Maybe this is an indication of error, and should be reported?
	}
	break;
	}
	byteIndex=0;
}

bool ServoConfigParser::parse(const uint8_t in)
{
	switch(step) {
	case(ACTUATOR_INDEX): {
		actuatorIndex=in;
		nextStep();
	}
	break;
	case(FLAGS): {
		converter.uint8[byteIndex]=in;
		byteIndex++;
		if(byteIndex==2) {
			currentConfig().flags=converter.uint16[0];
			nextStep();
		}
	}
	break;
	case(TYPE): {
		converter.uint8[byteIndex]=in;
		byteIndex++;
		if(byteIndex==2) {
			currentConfig().type=(ActuatorType)converter.uint16[0];
			nextStep();
		}
	}
	break;
	case(REPRESENTATION): {
		currentConfig().representation=(ActuatorRepresentation)in;
		nextStep();
	}
	break;
	case(NICK): {
		currentConfig().nickName[byteIndex]=(char)in;
		byteIndex++;
		if(byteIndex==20) {
			nextStep();
		}
	}
	break;

	case(GEAR_NUMERATOR): {
		currentConfig().gearRatioNumerator=in;
		nextStep();
	}
	break;


	case(GEAR_DENOMINATOR): {
		currentConfig().gearRatioDenominator=in;
		nextStep();
	}
	break;

	case(POSITION_FEEDBACK): {
		currentConfig().positionFeedbackPin=in;
		nextStep();
	}
	break;

	case(TACHOMETER): {
		currentConfig().tachometerPin=in;
		nextStep();
	}
	break;

	case(ENCODER_PIN_A): {
		currentConfig().incrementalEncoderPinA=in;
		nextStep();
	}
	break;

	case(ENCODER_PIN_B): {
		currentConfig().incrementalEncoderPinB=in;
		nextStep();
	}
	break;

	case(ENCODER_DEBOUNCE): {
		currentConfig().incrementalencoderDebounceCount=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_PIN_START): {
		currentConfig().limitSwitchPinStart=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_PIN_END): {
		currentConfig().limitSwitchPinEnd=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_DEBOUNCE): {
		currentConfig().limitSwitchDebounceCount=in;
		nextStep();
	}
	break;

	case(STEP_PHASE_COUNT): {
		currentConfig().stepMotorPhaseCount=in;
		nextStep();
	}
	break;

	case(STEP_STEPS_PER_ROTATION): {
		converter.uint8[byteIndex]=in;
		byteIndex++;
		if(byteIndex==2) {
			currentConfig().stepMotorStepsPerRotation=converter.uint16[0];
			nextStep();
		}
	}
	break;

	case(RC_SERVO_PIN): {
		currentConfig().rcServoPin=in;
		nextStep();
	}
	break;

	case(RANGE_START): {
		switch(currentConfig().representation) {
		case(ActuatorRepresentation::BIT): {
			currentConfig().rangeStart.bit=((in&1)>0);// Send bit as byte ignoring all but the first bit
			nextStep();
		}
		break;
		case(ActuatorRepresentation::BYTE): {
			currentConfig().rangeStart.byte=in;
			nextStep();
		}
		break;
		case(ActuatorRepresentation::WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==2) {
				currentConfig().rangeStart.word=converter.uint16[0];
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::DOUBLE_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				currentConfig().rangeStart.doubleWord=converter.uint32[0];
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::QUAD_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				currentConfig().rangeStart.quadWord=converter.uint64;
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::SINGLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				currentConfig().rangeStart.singlePrecision=converter.float32[0];
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::DOUBLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				currentConfig().rangeStart.doublePrecision=converter.float64;
				nextStep();
			}
		}
		break;
		default:
		case(ActuatorRepresentation::REPRESENTATION_COUNT): {
			// TODO: Handle this as an error somehow
		}
		break;
		}
	}
	break;
	case(RANGE_SPAN): {
		switch(currentConfig().representation) {
		case(ActuatorRepresentation::BIT): {
			currentConfig().rangeSpan.bit=((in&1)>0);// Send bit as byte ignoring all but the first bit
			nextStep();
		}
		break;
		case(ActuatorRepresentation::BYTE): {
			currentConfig().rangeSpan.byte=in;
			nextStep();
		}
		break;
		case(ActuatorRepresentation::WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==2) {
				currentConfig().rangeSpan.word=converter.uint16[0];
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::DOUBLE_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				currentConfig().rangeSpan.doubleWord=converter.uint32[0];
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::QUAD_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				currentConfig().rangeSpan.quadWord=converter.uint64;
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::SINGLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				currentConfig().rangeSpan.singlePrecision=converter.float32[0];
				nextStep();
			}
		}
		break;
		case(ActuatorRepresentation::DOUBLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				currentConfig().rangeSpan.doublePrecision=converter.float64;
				nextStep();
			}
		}
		break;
		default:
		case(ActuatorRepresentation::REPRESENTATION_COUNT): {
			// TODO: Handle this as an error somehow
		}
		break;
		}
	}
	break;
	}

	return (END_OF_PARSE==step);
}
