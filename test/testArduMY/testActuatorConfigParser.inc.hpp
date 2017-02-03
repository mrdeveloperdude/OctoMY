#ifndef TEST_INCLUDED

#include "test.inc.hpp"

void testActuatorConfigEqualsInclude(ActuatorSet &inSet)
{
#endif

	ArduMYActuatorConfigParser parser;
	parser.setConfig(c);
	Converter cv;

// Send flags
	QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::FLAGS);
	cv.uint16[0]=c.flags;
	parser.parse(cv.uint8[0]);
	QCOMPARE(parser.byteIndex, (uint16_t) 1);
	QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::FLAGS);
	parser.parse(cv.uint8[1]);
	QCOMPARE(parser.byteIndex, (uint16_t) 0);
	QCOMPARE(parser.config->flags, c.flags);
//qDebug()<<(uint16_t)parser.step<< (uint16_t)ServoConfigParser::TYPE;

// Send type
	QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::TYPE);
	cv.uint16[0]=(uint16_t)c.type;
	parser.parse(cv.uint8[0]);
	QCOMPARE(parser.byteIndex, (uint16_t) 1);
	QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::TYPE);
	parser.parse(cv.uint8[1]);
	QCOMPARE(parser.byteIndex, (uint16_t) 0);
	QCOMPARE(parser.config->type, c.type);
//qDebug()<<(uint16_t)parser.step<< (uint16_t)ServoConfigParser::REPRESENTATION;

// Send representation
	QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::REPRESENTATION);
	parser.parse((uint8_t)c.representation);
	QCOMPARE(parser.byteIndex, (uint16_t) 0);
	QCOMPARE(parser.config->representation, c.representation);

// Send nick
	QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::NICK);
	const int nickSize=sizeof(c.nickName)/sizeof(int8_t);
	int i=0;
//qDebug()<<"NICK SIZE: "<<nickSize;
	for(i=0; i<nickSize; ++i) {
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::NICK);
		QCOMPARE(parser.byteIndex, (uint16_t) i);
		parser.parse((uint8_t)c.nickName[i]);
	}
	QCOMPARE(parser.byteIndex, (uint16_t) 0);

	for(i=0; i<nickSize; ++i) {
		QCOMPARE(parser.config->nickName[i], c.nickName[i]);
	}

	if(c.hasGearRatio()) {
		// Send gear ratio numerator
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::GEAR_NUMERATOR);
		parser.parse(c.gearRatioNumerator);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->gearRatioNumerator, c.gearRatioNumerator);

		// Send gear ratio denominator
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::GEAR_DENOMINATOR);
		parser.parse(c.gearRatioDenominator);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->gearRatioDenominator, c.gearRatioDenominator);
	}

	if(c.hasPositionFeedback()) {
		// Send position feedback pin
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::POSITION_FEEDBACK);
		parser.parse(c.positionFeedbackPin);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->positionFeedbackPin, c.positionFeedbackPin);
	}

	if(c.hasTachometer()) {
		// Send tachometer pin
		//qDebug()<<(uint16_t)parser.step<< (uint16_t)ServoConfigParser::TACHOMETER;
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::TACHOMETER);
		parser.parse(c.tachometerPin);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->tachometerPin, c.tachometerPin);
	}


	if(c.hasIncrementalEncoder()) {
		// Send encoder pin A
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::ENCODER_PIN_A);
		parser.parse(c.incrementalEncoderPinA);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->incrementalEncoderPinA, c.incrementalEncoderPinA);

		// Send encoder pin B
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::ENCODER_PIN_B);
		parser.parse(c.incrementalEncoderPinB);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->incrementalEncoderPinB, c.incrementalEncoderPinB);

		// Send encoder debounce amount
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::ENCODER_DEBOUNCE);
		parser.parse(c.incrementalencoderDebounceCount);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->incrementalencoderDebounceCount, c.incrementalencoderDebounceCount);
	}

	if(c.hasLimitSwitchStart()) {
		// Send limit switch start pin
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::LIMIT_SWITCH_PIN_START);
		parser.parse(c.limitSwitchPinStart);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->limitSwitchPinStart, c.limitSwitchPinStart);
	}

	if(c.hasLimitSwitchEnd()) {
		// Send limit switch end pin
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::LIMIT_SWITCH_PIN_END);
		parser.parse(c.limitSwitchPinEnd);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->limitSwitchPinEnd, c.limitSwitchPinEnd);
	}

	if(c.hasLimitSwitchStart() || c.hasLimitSwitchEnd()) {
		// Send limit switch debounce count
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::LIMIT_SWITCH_DEBOUNCE);
		parser.parse(c.limitSwitchDebounceCount);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->limitSwitchDebounceCount, c.limitSwitchDebounceCount);
	}

	switch(parser.config->type) {
	case(ArduMYActuatorType::DC_MOTOR): {
	} break;
	case(ArduMYActuatorType::STEP_MOTOR): {
		// Send step motor phase count
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::STEP_PHASE_COUNT);
		parser.parse(c.stepMotorPhaseCount);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->stepMotorPhaseCount, c.stepMotorPhaseCount);

		// Send step motor steps per rotation
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::STEP_STEPS_PER_ROTATION);
		cv.uint16[0]=c.stepMotorStepsPerRotation;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::STEP_STEPS_PER_ROTATION);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->stepMotorStepsPerRotation, c.stepMotorStepsPerRotation);
	}
	break;
	case(ArduMYActuatorType::RC_SERVO): {
		// Send RC servo pwm pin
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RC_SERVO_PIN);
		parser.parse(c.rcServoPin);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rcServoPin, c.rcServoPin);
	}
	break;
	case(ArduMYActuatorType::RELAY): {
	} break;
	default:
	case(ArduMYActuatorType::TYPE_COUNT): {
		QFAIL("bad type");
	}
	break;
	}


//qDebug()<<"STEP "<< parser.step << ServoConfigParser::RANGE_START;
// Send range start
	switch(parser.config->representation) {
	case(ArduMYActuatorValueRepresentation::BIT): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(c.rangeStart.bit?(uint8_t)1:(uint8_t)0);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.bit, c.rangeStart.bit);
	}
	break;
	case(ArduMYActuatorValueRepresentation::BYTE): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(c.rangeStart.byte);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.byte, c.rangeStart.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::WORD): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		cv.uint16[0]=c.rangeStart.word;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.word, c.rangeStart.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		cv.uint32[0]=c.rangeStart.doubleWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.doubleWord, c.rangeStart.doubleWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		cv.uint64=c.rangeStart.quadWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.quadWord, c.rangeStart.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		cv.float32[0]=c.rangeStart.singlePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.singlePrecision, c.rangeStart.singlePrecision);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_START);
		cv.float64=c.rangeStart.doublePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.doublePrecision, c.rangeStart.doublePrecision);
	}
	break;
	default:
	case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT): {
		QFAIL("bad representation for range start");
	}
	break;
	}


// Send range span
	switch(parser.config->representation) {
	case(ArduMYActuatorValueRepresentation::BIT): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(c.rangeSpan.bit?(uint8_t)1:(uint8_t)0);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.bit, c.rangeSpan.bit);
	}
	break;
	case(ArduMYActuatorValueRepresentation::BYTE): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(c.rangeSpan.byte);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.byte, c.rangeSpan.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::WORD): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		cv.uint16[0]=c.rangeSpan.word;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.word, c.rangeSpan.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		cv.uint32[0]=c.rangeSpan.doubleWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.doubleWord, c.rangeSpan.doubleWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		cv.uint64=c.rangeSpan.quadWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.quadWord, c.rangeSpan.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		cv.float32[0]=c.rangeSpan.singlePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.singlePrecision, c.rangeSpan.singlePrecision);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
		QCOMPARE(parser.step,  ArduMYActuatorConfigParserStep::RANGE_SPAN);
		cv.float64=c.rangeSpan.doublePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ArduMYActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.doublePrecision, c.rangeSpan.doublePrecision);
	}
	break;
	default:
	case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT): {
		QFAIL("bad representation for range span");
	}
	break;
	}

#ifndef TEST_INCLUDED
}
#endif
