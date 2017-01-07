#include "TestArduMY.hpp"
#include "utility/Utility.hpp"

#include <QDebug>

void TestArduMY::testMagicDetector()
{
	static const uint8_t magic[]= {0xFF, 0x4E, 0x23, 0x7C, 0xAB};
	MagicDetector magicDetector(magic,5);

	static const uint8_t positiveTestSequence[]= {0xFF, 0x4E, 0xDF, 0x23, 0x7C, 0x1B, 0xFD, 0x4E, 0xDF, 0x7C, 0xAB, 0x23, 0xFF, 0x4E, 0xD4, 0x23, 0xFF, 0x4E, 0x23, 0x7C, 0xAB, 0x7D, 0xAB};
	uint32_t ct=0;
	for(uint8_t in: positiveTestSequence) {
		if(magicDetector.detect(in)) {
			QCOMPARE(ct, (uint32_t)20);
			break;
		}
		ct++;
	}
	static const uint8_t negativeTestSequence[]= {0xFF, 0x4E, 0xDF, 0x23, 0x7C, 0x1B, 0xFD, 0x4E, 0xDF, 0x7C, 0xAB, 0x23, 0xFF, 0x4E, 0xD4, 0x23, 0xFF, 0x43, 0x23, 0x7C, 0xAB, 0x7D, 0xAB};
	ct=0;
	for(uint8_t in: negativeTestSequence) {
		if(magicDetector.detect(in)) {
			break;
		}
		ct++;
	}
	QCOMPARE(ct, (uint32_t)23);

}

const bool TestArduMY::percentChance(quint8 pct) const
{
	return (pct>=(qrand()%100));
}



static QString randomString(int sz)
{
	static const QString alphabet="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!\"#¤%&/()= _-.,:;^æøåÆÅØ";
	static const int alphalen=alphabet.size();
	QString out;
	for(int i=0; i<sz; ++i) {
		out.append(alphabet[ qrand()%alphalen ]);
	}
	return out;
}

static float frand()
{
	float v=qrand();
	v/=RAND_MAX;
	float v2=qrand()-qrand();
	v*=v2;
	return v;
}

static double dfrand()
{
	double v=qrand();
	v/=RAND_MAX;
	double v2=qrand()-qrand();
	v*=v2;
	return v;
}

ActuatorConfig TestArduMY::randomConfig() const
{
	ActuatorConfig c;
	// Set random flags
	c.setLinear(percentChance(50));
	c.setContinuous(percentChance(50));
	c.setGearRatio(percentChance(50));
	c.setPositionFeedback(percentChance(50));
	c.setTachometer(percentChance(50));
	c.setIncrementalEncoder(percentChance(50));
	c.setLimitSwitchStart(percentChance(50));
	c.setLimitSwitchEnd(percentChance(50));
	c.setDirty(percentChance(50));

	// Set random type
	c.type=(ActuatorType)(qrand() % ((quint8)ActuatorType::TYPE_COUNT));

	// Set random representation
	c.representation=(ActuatorRepresentation)(qrand() % ((quint8)ActuatorRepresentation::REPRESENTATION_COUNT));

	// Set random name
	const quint8 nameSize=(qrand()%20);
	QString name=randomString(nameSize);
	auto name_cstr=name.toStdString().c_str();
	int i=0;
	for(i=0; i<nameSize; ++i) {
		c.nickName[i]=name_cstr[i];
	}
	// Fill rest of string with \0
	for(; i<20; ++i) {
		c.nickName[i]='\0';
	}
	// Set random gear ratio
	c.gearRatioNumerator=1+(qrand()%100);
	c.gearRatioDenominator=1+(qrand()%200);

	// Set random debounce counts
	c.incrementalencoderDebounceCount=qrand()%20;
	c.limitSwitchDebounceCount=qrand()%20;

	// Set random hardware pins
	c.incrementalEncoderPinA=qrand()%54;
	c.incrementalEncoderPinB=qrand()%54;
	c.limitSwitchPinStart=qrand()%54;
	c.limitSwitchPinEnd=qrand()%54;
	c.positionFeedbackPin=qrand()%54;
	c.rcServoPin=qrand()%54;
	c.tachometerPin=qrand()%54;

	// Set random step motor phase count
	c.stepMotorPhaseCount=1+(qrand()%4);

	// Set random step motor steps per rotation
	c.stepMotorPhaseCount=90+(qrand()%145);

	// Set random range
	switch(c.representation) {
	case(ActuatorRepresentation::BIT): {
		c.rangeStart.byte=0;
		c.rangeSpan.byte=0;
		c.rangeStart.bit=false;
		c.rangeSpan.bit=true;
	}
	break;
	case(ActuatorRepresentation::BYTE): {
		c.rangeStart.byte=qrand()%qrand()%(0xFF);
		c.rangeSpan.byte=qrand()%((0xFF)-c.rangeStart.byte);
	}
	break;
	case(ActuatorRepresentation::WORD): {
		c.rangeStart.word=qrand()%qrand()%(0xFFFF);
		c.rangeSpan.word=qrand()%((0xFFFF)-c.rangeStart.word);
	}
	break;
	case(ActuatorRepresentation::DOUBLE_WORD): {
		c.rangeStart.doubleWord=qrand()%qrand()%(0xFFFFFFFF);
		c.rangeSpan.doubleWord=qrand()%((0xFFFFFFFF)-c.rangeStart.doubleWord);
	}
	break;
	case(ActuatorRepresentation::QUAD_WORD): {
		// Naive attempt at making 64bit random numbers using qrand()
		uint64_t r1=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		uint64_t r2=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		uint64_t r3=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		c.rangeStart.quadWord=r1%r2%(0xFFFFFFFFFFFFFFFF);
		c.rangeSpan.quadWord=r3%((0xFFFFFFFFFFFFFFFF)-c.rangeStart.quadWord);
	}
	break;
	case(ActuatorRepresentation::SINGLE_FLOAT): {
		float r1=frand();
		float r2=frand();
		float r3=frand();
		c.rangeStart.singlePrecision=r1*r2;
		c.rangeSpan.singlePrecision=r3-c.rangeStart.singlePrecision;
	}
	break;
	case(ActuatorRepresentation::DOUBLE_FLOAT): {
		double r1=dfrand();
		double r2=dfrand();
		double r3=dfrand();
		c.rangeStart.doublePrecision=r1*r2;
		c.rangeSpan.doublePrecision=r3-c.rangeStart.doublePrecision;
	}
	break;
	default:
	case(ActuatorRepresentation::REPRESENTATION_COUNT): {
		qWarning()<<"bad representation";
	}
	break;
	}

	// Return resulting random config
	return c;
}




void TestArduMY::singleServoConfigRun(const ActuatorConfig &c) const
{
	CommandParser commandParser;
	Converter cv;

	// Conduct sync
	for(uint8_t i=0; i<4; ++i) {
		QCOMPARE(commandParser.currentCommand, OCTOMY_SYNC);
		QCOMPARE(commandParser.magicDetector.idx, i);
		commandParser.parse(commandParser.magic[i]);
	}

	// Verify that sync completed correctly
	QCOMPARE(commandParser.currentCommand, OCTOMY_AWAITING_COMMAND);

	// Start the actuator config command
	ParserState parserState=OCTOMY_SET_ACTUATOR_CONFIG;
	commandParser.parse((uint8_t)parserState);
	QCOMPARE(commandParser.currentCommand, parserState);

	// Send actuator index
	static const uint8_t actuatorIndex=12;
	commandParser.parse(actuatorIndex);
	QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);

	// Send flags
	QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::FLAGS);
	cv.uint16[0]=c.flags;
	commandParser.parse(cv.uint8[0]);
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
	QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::FLAGS);
	commandParser.parse(cv.uint8[1]);
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
	QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
	QCOMPARE(commandParser.servoConfigParser.currentConfig().flags, c.flags);
	//qDebug()<<(uint16_t)commandParser.servoConfigParser.step<< (uint16_t)ServoConfigParser::TYPE;

	// Send type
	QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::TYPE);

	cv.uint16[0]=(uint16_t)c.type;
	commandParser.parse(cv.uint8[0]);
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
	QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::TYPE);
	commandParser.parse(cv.uint8[1]);
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
	QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
	QCOMPARE(commandParser.servoConfigParser.currentConfig().type, c.type);
	//qDebug()<<(uint16_t)commandParser.servoConfigParser.step<< (uint16_t)ServoConfigParser::REPRESENTATION;

	// Send representation
	QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::REPRESENTATION);
	commandParser.parse((uint8_t)c.representation);
	QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
	QCOMPARE(commandParser.servoConfigParser.currentConfig().representation, c.representation);

	// Send nick
	QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::NICK);
	const int nickSize=sizeof(c.nickName)/sizeof(int8_t);
	int i=0;
	//qDebug()<<"NICK SIZE: "<<nickSize;
	for(i=0; i<nickSize; ++i) {
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::NICK);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) i);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		commandParser.parse((uint8_t)c.nickName[i]);
	}
	QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
	QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
	for(i=0; i<nickSize; ++i) {
		QCOMPARE(commandParser.servoConfigParser.currentConfig().nickName[i], c.nickName[i]);
	}

	if(c.hasGearRatio()) {
		// Send gear ratio numerator
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::GEAR_NUMERATOR);
		commandParser.parse(c.gearRatioNumerator);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().gearRatioNumerator, c.gearRatioNumerator);

		// Send gear ratio denominator
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::GEAR_DENOMINATOR);
		commandParser.parse(c.gearRatioDenominator);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().gearRatioDenominator, c.gearRatioDenominator);
	}

	if(c.hasPositionFeedback()) {
		// Send position feedback pin
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::POSITION_FEEDBACK);
		commandParser.parse(c.positionFeedbackPin);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().positionFeedbackPin, c.positionFeedbackPin);
	}

	if(c.hasTachometer()) {
		// Send tachometer pin
		//qDebug()<<(uint16_t)commandParser.servoConfigParser.step<< (uint16_t)ServoConfigParser::TACHOMETER;
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::TACHOMETER);
		commandParser.parse(c.tachometerPin);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().tachometerPin, c.tachometerPin);
	}


	if(c.hasIncrementalEncoder()) {
		// Send encoder pin A
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::ENCODER_PIN_A);
		commandParser.parse(c.incrementalEncoderPinA);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().incrementalEncoderPinA, c.incrementalEncoderPinA);

		// Send encoder pin B
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::ENCODER_PIN_B);
		commandParser.parse(c.incrementalEncoderPinB);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().incrementalEncoderPinB, c.incrementalEncoderPinB);

		// Send encoder debounce amount
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::ENCODER_DEBOUNCE);
		commandParser.parse(c.incrementalencoderDebounceCount);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().incrementalencoderDebounceCount, c.incrementalencoderDebounceCount);
	}

	if(c.hasLimitSwitchStart()) {
		// Send limit switch start pin
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::LIMIT_SWITCH_PIN_START);
		commandParser.parse(c.limitSwitchPinStart);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().limitSwitchPinStart, c.limitSwitchPinStart);
	}

	if(c.hasLimitSwitchEnd()) {
		// Send limit switch end pin
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::LIMIT_SWITCH_PIN_END);
		commandParser.parse(c.limitSwitchPinEnd);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().limitSwitchPinEnd, c.limitSwitchPinEnd);
	}

	if(c.hasLimitSwitchStart() || c.hasLimitSwitchEnd()) {
		// Send limit switch debounce count
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::LIMIT_SWITCH_DEBOUNCE);
		commandParser.parse(c.limitSwitchDebounceCount);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().limitSwitchDebounceCount, c.limitSwitchDebounceCount);
	}

	switch(commandParser.servoConfigParser.currentConfig().type) {
	case(ActuatorType::DC_MOTOR): {
	} break;
	case(ActuatorType::STEP_MOTOR): {
		// Send step motor phase count
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::STEP_PHASE_COUNT);
		commandParser.parse(c.stepMotorPhaseCount);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().stepMotorPhaseCount, c.stepMotorPhaseCount);

		// Send step motor steps per rotation
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::STEP_STEPS_PER_ROTATION);
		cv.uint16[0]=c.stepMotorStepsPerRotation;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::STEP_STEPS_PER_ROTATION);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().stepMotorStepsPerRotation, c.stepMotorStepsPerRotation);
	}
	break;
	case(ActuatorType::RC_SERVO): {
		// Send RC servo pwm pin
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RC_SERVO_PIN);
		commandParser.parse(c.rcServoPin);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rcServoPin, c.rcServoPin);
	}
	break;
	case(ActuatorType::RELAY): {
	} break;
	default:
	case(ActuatorType::TYPE_COUNT): {
		QFAIL("bad type");
	}
	break;
	}

//ACTUATOR_INDEX, FLAGS, TYPE, REPRESENTATION, NICK, GEAR_NUMERATOR, GEAR_DENOMINATOR, POSITION_FEEDBACK, TACHOMETER, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_DEBOUNCE, LIMIT_SWITCH_PIN_START, LIMIT_SWITCH_PIN_END, LIMIT_SWITCH_DEBOUNCE, STEP_PHASE_COUNT, STEP_STEPS_PER_ROTATION, RC_SERVO_PIN, RANGE_START, RANGE_SPAN, END_OF_PARSE

//qDebug()<<"STEP "<< commandParser.servoConfigParser.step << ServoConfigParser::RANGE_START;
	// Send range start
	switch(commandParser.servoConfigParser.currentConfig().representation) {
	case(ActuatorRepresentation::BIT): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		commandParser.parse(c.rangeStart.bit?(uint8_t)1:(uint8_t)0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.bit, c.rangeStart.bit);
	}
	break;
	case(ActuatorRepresentation::BYTE): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		commandParser.parse(c.rangeStart.byte);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.byte, c.rangeStart.byte);
	}
	break;
	case(ActuatorRepresentation::WORD): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		cv.uint16[0]=c.rangeStart.word;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.word, c.rangeStart.word);
	}
	break;
	case(ActuatorRepresentation::DOUBLE_WORD): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		cv.uint32[0]=c.rangeStart.doubleWord;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.doubleWord, c.rangeStart.doubleWord);
	}
	break;
	case(ActuatorRepresentation::QUAD_WORD): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		cv.uint64=c.rangeStart.quadWord;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 4);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[4]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 5);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[5]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 6);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[6]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 7);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[7]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.quadWord, c.rangeStart.quadWord);
	}
	break;
	case(ActuatorRepresentation::SINGLE_FLOAT): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		cv.float32[0]=c.rangeStart.singlePrecision;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.singlePrecision, c.rangeStart.singlePrecision);
	}
	break;
	case(ActuatorRepresentation::DOUBLE_FLOAT): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_START);
		cv.float64=c.rangeStart.doublePrecision;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 4);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[4]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 5);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[5]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 6);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[6]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 7);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_START);
		commandParser.parse(cv.uint8[7]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeStart.doublePrecision, c.rangeStart.doublePrecision);
	}
	break;
	default:
	case(ActuatorRepresentation::REPRESENTATION_COUNT): {
		QFAIL("bad representation for range start");
	}
	break;
	}


	// Send range span
	switch(commandParser.servoConfigParser.currentConfig().representation) {
	case(ActuatorRepresentation::BIT): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		commandParser.parse(c.rangeSpan.bit?(uint8_t)1:(uint8_t)0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.bit, c.rangeSpan.bit);
	}
	break;
	case(ActuatorRepresentation::BYTE): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		commandParser.parse(c.rangeSpan.byte);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.byte, c.rangeSpan.byte);
	}
	break;
	case(ActuatorRepresentation::WORD): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		cv.uint16[0]=c.rangeSpan.word;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.word, c.rangeSpan.word);
	}
	break;
	case(ActuatorRepresentation::DOUBLE_WORD): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		cv.uint32[0]=c.rangeSpan.doubleWord;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.doubleWord, c.rangeSpan.doubleWord);
	}
	break;
	case(ActuatorRepresentation::QUAD_WORD): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		cv.uint64=c.rangeSpan.quadWord;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 4);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[4]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 5);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[5]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 6);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[6]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 7);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[7]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.quadWord, c.rangeSpan.quadWord);
	}
	break;
	case(ActuatorRepresentation::SINGLE_FLOAT): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		cv.float32[0]=c.rangeSpan.singlePrecision;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.singlePrecision, c.rangeSpan.singlePrecision);
	}
	break;
	case(ActuatorRepresentation::DOUBLE_FLOAT): {
		QCOMPARE(commandParser.servoConfigParser.step,  ServoConfigParser::RANGE_SPAN);
		cv.float64=c.rangeSpan.doublePrecision;
		commandParser.parse(cv.uint8[0]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 1);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[1]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 2);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[2]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 3);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[3]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 4);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[4]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 5);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[5]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 6);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[6]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 7);
		QCOMPARE(commandParser.servoConfigParser.step, ServoConfigParser::RANGE_SPAN);
		commandParser.parse(cv.uint8[7]);
		QCOMPARE(commandParser.servoConfigParser.byteIndex, (uint16_t) 0);
		QCOMPARE(commandParser.servoConfigParser.actuatorIndex, actuatorIndex);
		QCOMPARE(commandParser.servoConfigParser.currentConfig().rangeSpan.doublePrecision, c.rangeSpan.doublePrecision);
	}
	break;
	default:
	case(ActuatorRepresentation::REPRESENTATION_COUNT): {
		QFAIL("bad representation for range span");
	}
	break;
	}

}

void TestArduMY::testServoConfigParser()
{
	// Generate 1000 random configurations and feed them through a parser to verify the parser works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Round "<<j<<" / 100";
		for(int i=0; i<1000; ++i) {
			ActuatorConfig c=randomConfig();
			singleServoConfigRun(c);
		}
	}

}

void TestArduMY::testServoPositionParser()
{
	//SetServoPositionsParser servoPosParser;
}


QTEST_MAIN(TestArduMY)
