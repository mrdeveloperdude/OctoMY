#include "ardumy_arduino/ArduMYActuatorConfigSerializer.hpp"

#include "ardumy_arduino/ArduMY.hpp"


ArduMYActuatorConfigSerializer::ArduMYActuatorConfigSerializer()
	: ArduMYActuatorConfigSerializerBase()
{

}


// Return true if there is more data remaining
bool ArduMYActuatorConfigSerializer::hasMoreData() const
{
	return (step!=END_OF_OPERATION);
}

// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
uint8_t ArduMYActuatorConfigSerializer::nextByte()
{
	uint8_t ret=0x00;
	switch(step) {
	case(FLAGS): {
		// Serialize flags
		converter.uint16[0]=config->flags;
		ret=converter.uint8[byteIndex];
		//qDebug()<<"CONFIG SERIALIZE FLAGS @"<<byteIndex<<": "<<ret;
		byteIndex++;
		if(2==byteIndex) {
			nextStep();
		}
	}
	break;
	case(TYPE): {
		// Serialize type
		converter.uint16[0]=(uint16_t)config->type;
		ret=converter.uint8[byteIndex];
		//qDebug()<<"CONFIG SERIALIZE TYPE @"<<byteIndex<<": "<<ret;
		byteIndex++;
		if(2==byteIndex) {
			nextStep();
		}
	}
	break;
	case(REPRESENTATION): {
		// Serialize representation
		ret=(uint8_t)config->representation;
		nextStep();
	}
	break;
	case(NICK): {
		// Serialize nick name
		const int nickSize=sizeof(config->nickName) / sizeof(int8_t);
		ret=(uint8_t)config->nickName[byteIndex];
		byteIndex++;
		if(byteIndex == nickSize) {
			//qDebug()<<"------- - - -- - - - "<<QString::fromLocal8Bit((char *)config->nickName,20);
			nextStep();
		}
	}
	break;
	case(GEAR_NUMERATOR): {
		// Serialize gear ratio numerator
		ret=config->gearRatioNumerator;
		nextStep();
	}
	break;
	case(GEAR_DENOMINATOR): {
		// Serialize gear ratio denominator
		ret=config->gearRatioDenominator;
		nextStep();
	}
	break;
	case(POSITION_FEEDBACK): {
		// Serialize position feedback pin
		ret=config->positionFeedbackPin;
		nextStep();
	}
	break;
	case(TACHOMETER): {
		// Serialize tachometer pin
		ret=config->tachometerPin;
		nextStep();
	}
	break;
	case(ENCODER_PIN_A): {
		// Serialize encoder pin A
		ret=config->incrementalEncoderPinA;
		nextStep();
	}
	break;
	case(ENCODER_PIN_B): {
		// Serialize encoder pin B
		ret=config->incrementalEncoderPinB;
		nextStep();
	}
	break;
	case(ENCODER_DEBOUNCE): {
		// Serialize encoder debounce amount
		ret=config->incrementalencoderDebounceCount;
		nextStep();
	}
	break;
	case(LIMIT_SWITCH_PIN_START): {
		// Serialize limit switch start pin
		ret=config->limitSwitchPinStart;
		nextStep();
	}
	break;
	case(LIMIT_SWITCH_PIN_END): {
		// Serialize limit switch end pin
		ret=config->limitSwitchPinEnd;
		nextStep();
	}
	break;
	case(LIMIT_SWITCH_DEBOUNCE): {
		// Serialize limit switch debounce count
		ret=config->limitSwitchDebounceCount;
		nextStep();
	}
	break;
	case(STEP_PHASE_COUNT): {
		// Serialize step motor phase count
		ret=config->stepMotorPhaseCount;
		nextStep();
	}
	break;
	case(STEP_STEPS_PER_ROTATION): {
		// Serialize step motor steps per rotation
		converter.uint16[0]=config->stepMotorStepsPerRotation;
		ret=converter.uint8[byteIndex];
		byteIndex++;
		if(2==byteIndex) {
			nextStep();
		}
	}
	break;
	case(RC_SERVO_PIN): {
		// Serialize RC servo pwm pin
		ret=config->rcServoPin;
		nextStep();
	}
	break;
	case(RANGE_START): {
		// Serialize range start
		switch(config->representation) {
		case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
			//qDebug()<<"RANGE START BIT BYTE INDEX: "<<byteIndex;
			ret=config->rangeStart.bit?(uint8_t)1:(uint8_t)0;
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
			//qDebug()<<"RANGE START BYTE BYTE INDEX: "<<byteIndex;
			ret=config->rangeStart.byte;
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
			//qDebug()<<"RANGE START WORD BYTE INDEX: "<<byteIndex;
			converter.uint16[0]=config->rangeStart.word;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(2==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
			//qDebug()<<"RANGE START DWORD BYTE INDEX: "<<byteIndex;
			converter.uint32[0]=config->rangeStart.doubleWord;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(4==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
			//qDebug()<<"RANGE START QWORD BYTE INDEX: "<<byteIndex;
			converter.uint64=config->rangeStart.quadWord;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(8==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
			//qDebug()<<"RANGE START FLOAT BYTE INDEX: "<<byteIndex;
			converter.float32[0]=config->rangeStart.singlePrecision;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(4==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
			//qDebug()<<"RANGE START DOUBLE BYTE INDEX: "<<byteIndex;
			converter.float64=config->rangeStart.doublePrecision;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(8==byteIndex) {
				nextStep();
			}
		}
		break;
		default:
		case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT): {
			//qDebug()<<"RANGE START BAD BYTE INDEX: "<<byteIndex;
			//TODO: Handle this as an error somwhow
		}
		break;
		}
	}
	break;
	case(RANGE_SPAN): {
		// Serialize range span
		switch(config->representation) {
		case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
			//qDebug()<<"RANGE END BIT BYTE INDEX: "<<byteIndex;
			ret=config->rangeSpan.bit?(uint8_t)1:(uint8_t)0;
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
			//qDebug()<<"RANGE END BYTE BYTE INDEX: "<<byteIndex;
			ret=config->rangeSpan.byte;
			nextStep();
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
			//qDebug()<<"RANGE END WORD BYTE INDEX: "<<byteIndex;
			converter.uint16[0]=config->rangeSpan.word;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(2==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
			//qDebug()<<"RANGE END DWORD BYTE INDEX: "<<byteIndex;
			converter.uint32[0]=config->rangeSpan.doubleWord;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(4==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
			//qDebug()<<"RANGE END QWORD BYTE INDEX: "<<byteIndex;
			converter.uint64=config->rangeSpan.quadWord;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(8==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
			//qDebug()<<"RANGE END FLOAT BYTE INDEX: "<<byteIndex;
			converter.float32[0]=config->rangeSpan.singlePrecision;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(4==byteIndex) {
				nextStep();
			}
		}
		break;
		case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
			//qDebug()<<"RANGE END DOUBLE BYTE INDEX: "<<byteIndex;
			converter.float64=config->rangeSpan.doublePrecision;
			ret=converter.uint8[byteIndex];
			byteIndex++;
			if(8==byteIndex) {
				nextStep();
			}
		}
		break;
		default:
		case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT): {
			//qDebug()<<"RANGE END BAD BYTE INDEX: "<<byteIndex;
			//TODO: Handle this as an error somwhow
		}
		break;
		}
	}
	break;
	default:
	case(END_OF_OPERATION): {
		qWarning()<<"WARNING: NEXT BYTE HAD END OF OPERATION";
		ret=0x00;
	}
	break;
	}

	return ret;
}
