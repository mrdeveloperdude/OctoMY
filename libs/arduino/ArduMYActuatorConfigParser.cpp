#include "ArduMYActuatorConfigParser.hpp"

#include "CommandParser.hpp"


ArduMYActuatorConfigParser::ArduMYActuatorConfigParser()
	: ArduMYActuatorConfigSerializerBase()
{

}

bool ArduMYActuatorConfigParser::parse(const uint8_t in)
{
	Q_CHECK_PTR(config);
	switch(step) {
	case(FLAGS): {
		converter.uint8[byteIndex]=in;
		byteIndex++;
		if(byteIndex==2) {
			config->flags=converter.uint16[0];
			nextStep();
		}
	}
	break;
	case(TYPE): {
		converter.uint8[byteIndex]=in;
		byteIndex++;
		if(byteIndex==2) {
			config->type=(ArduMYActuatorType)converter.uint16[0];
			nextStep();
		}
	}
	break;
	case(REPRESENTATION): {
		config->representation=(ArduMYActuatorValueRepresentation)in;
		nextStep();
	}
	break;
	case(NICK): {
		config->nickName[byteIndex]=(char)in;
		byteIndex++;
		if(byteIndex==20) {
			nextStep();
		}
	}
	break;

	case(GEAR_NUMERATOR): {
		config->gearRatioNumerator=in;
		nextStep();
	}
	break;
	case(GEAR_DENOMINATOR): {
		config->gearRatioDenominator=in;
		nextStep();
	}
	break;

	case(POSITION_FEEDBACK): {
		config->positionFeedbackPin=in;
		nextStep();
	}
	break;

	case(TACHOMETER): {
		config->tachometerPin=in;
		nextStep();
	}
	break;

	case(ENCODER_PIN_A): {
		config->incrementalEncoderPinA=in;
		nextStep();
	}
	break;

	case(ENCODER_PIN_B): {
		config->incrementalEncoderPinB=in;
		nextStep();
	}
	break;

	case(ENCODER_DEBOUNCE): {
		config->incrementalencoderDebounceCount=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_PIN_START): {
		config->limitSwitchPinStart=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_PIN_END): {
		config->limitSwitchPinEnd=in;
		nextStep();
	}
	break;

	case(LIMIT_SWITCH_DEBOUNCE): {
		config->limitSwitchDebounceCount=in;
		nextStep();
	}
	break;

	case(STEP_PHASE_COUNT): {
		config->stepMotorPhaseCount=in;
		nextStep();
	}
	break;

	case(STEP_STEPS_PER_ROTATION): {
		converter.uint8[byteIndex]=in;
		byteIndex++;
		if(byteIndex==2) {
			config->stepMotorStepsPerRotation=converter.uint16[0];
			nextStep();
		}
	}
	break;

	case(RC_SERVO_PIN): {
		config->rcServoPin=in;
		nextStep();
	}
	break;

	case(RANGE_START): {
		switch(config->representation) {
		case(ArduMYActuatorValueRepresentation::BIT): {
			config->rangeStart.bit=((in&1)>0);// Send bit as byte ignoring all but the first bit
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::BYTE): {
			config->rangeStart.byte=in;
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==2) {
				config->rangeStart.word=converter.uint16[0];
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				config->rangeStart.doubleWord=converter.uint32[0];
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				config->rangeStart.quadWord=converter.uint64;
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				config->rangeStart.singlePrecision=converter.float32[0];
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				config->rangeStart.doublePrecision=converter.float64;
				nextStep();
			}
		}
		break;
		default:
		case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT): {
			// TODO: Handle this as an error somehow
		}
		break;
		}
	}
	break;
	case(RANGE_SPAN): {
		switch(config->representation) {
		case(ArduMYActuatorValueRepresentation::BIT): {
			config->rangeSpan.bit=((in&1)>0);// Send bit as byte ignoring all but the first bit
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::BYTE): {
			config->rangeSpan.byte=in;
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==2) {
				config->rangeSpan.word=converter.uint16[0];
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				config->rangeSpan.doubleWord=converter.uint32[0];
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				config->rangeSpan.quadWord=converter.uint64;
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==4) {
				config->rangeSpan.singlePrecision=converter.float32[0];
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
			converter.uint8[byteIndex]=in;
			byteIndex++;
			if(byteIndex==8) {
				config->rangeSpan.doublePrecision=converter.float64;
				nextStep();
			}
		}
		break;
		default:
		case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT): {
			// TODO: Handle this as an error somehow
		}
		break;
		}
	}
	break;
	default:
	case(END_OF_OPERATION): {
	}
	break;
	}

	return (END_OF_OPERATION==step);
}
