#include "TestArduMY.hpp"
#include "utility/Utility.hpp"

#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"

#include "../arduino/MagicDetector.hpp"

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

static bool percentChance(quint8 pct)
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
	c.representation=(ActuatorValueRepresentation)(qrand() % ((quint8)ActuatorValueRepresentation::REPRESENTATION_COUNT));

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
	case(ActuatorValueRepresentation::BIT): {
		c.rangeStart.byte=0;
		c.rangeSpan.byte=0;
		c.rangeStart.bit=false;
		c.rangeSpan.bit=true;
	}
	break;
	case(ActuatorValueRepresentation::BYTE): {
		c.rangeStart.byte=qrand()%qrand()%(0xFF);
		c.rangeSpan.byte=qrand()%((0xFF)-c.rangeStart.byte);
	}
	break;
	case(ActuatorValueRepresentation::WORD): {
		c.rangeStart.word=qrand()%qrand()%(0xFFFF);
		c.rangeSpan.word=qrand()%((0xFFFF)-c.rangeStart.word);
	}
	break;
	case(ActuatorValueRepresentation::DOUBLE_WORD): {
		c.rangeStart.doubleWord=qrand()%qrand()%(0xFFFFFFFF);
		c.rangeSpan.doubleWord=qrand()%((0xFFFFFFFF)-c.rangeStart.doubleWord);
	}
	break;
	case(ActuatorValueRepresentation::QUAD_WORD): {
		// Naive attempt at making 64bit random numbers using qrand()
		uint64_t r1=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		uint64_t r2=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		uint64_t r3=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		c.rangeStart.quadWord=r1%r2%(0xFFFFFFFFFFFFFFFF);
		c.rangeSpan.quadWord=r3%((0xFFFFFFFFFFFFFFFF)-c.rangeStart.quadWord);
	}
	break;
	case(ActuatorValueRepresentation::SINGLE_FLOAT): {
		float r1=frand();
		float r2=frand();
		float r3=frand();
		c.rangeStart.singlePrecision=r1*r2;
		c.rangeSpan.singlePrecision=r3-c.rangeStart.singlePrecision;
	}
	break;
	case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
		double r1=dfrand();
		double r2=dfrand();
		double r3=dfrand();
		c.rangeStart.doublePrecision=r1*r2;
		c.rangeSpan.doublePrecision=r3-c.rangeStart.doublePrecision;
	}
	break;
	default:
	case(ActuatorValueRepresentation::REPRESENTATION_COUNT): {
		qWarning()<<"bad representation";
	}
	break;
	}

	// Return resulting random config
	return c;
}




void TestArduMY::singleActuatorConfigParserRun(ActuatorConfig &c) const
{
	ActuatorConfigParser parser;
	parser.setConfig(c);
	Converter cv;

	// Send flags
	QCOMPARE(parser.step, ActuatorConfigParserStep::FLAGS);
	cv.uint16[0]=c.flags;
	parser.parse(cv.uint8[0]);
	QCOMPARE(parser.byteIndex, (uint16_t) 1);
	QCOMPARE(parser.step, ActuatorConfigParserStep::FLAGS);
	parser.parse(cv.uint8[1]);
	QCOMPARE(parser.byteIndex, (uint16_t) 0);
	QCOMPARE(parser.config->flags, c.flags);
	//qDebug()<<(uint16_t)parser.step<< (uint16_t)ServoConfigParser::TYPE;

	// Send type
	QCOMPARE(parser.step,  ActuatorConfigParserStep::TYPE);
	cv.uint16[0]=(uint16_t)c.type;
	parser.parse(cv.uint8[0]);
	QCOMPARE(parser.byteIndex, (uint16_t) 1);
	QCOMPARE(parser.step, ActuatorConfigParserStep::TYPE);
	parser.parse(cv.uint8[1]);
	QCOMPARE(parser.byteIndex, (uint16_t) 0);
	QCOMPARE(parser.config->type, c.type);
	//qDebug()<<(uint16_t)parser.step<< (uint16_t)ServoConfigParser::REPRESENTATION;

	// Send representation
	QCOMPARE(parser.step,  ActuatorConfigParserStep::REPRESENTATION);
	parser.parse((uint8_t)c.representation);
	QCOMPARE(parser.byteIndex, (uint16_t) 0);
	QCOMPARE(parser.config->representation, c.representation);

	// Send nick
	QCOMPARE(parser.step, ActuatorConfigParserStep::NICK);
	const int nickSize=sizeof(c.nickName)/sizeof(int8_t);
	int i=0;
	//qDebug()<<"NICK SIZE: "<<nickSize;
	for(i=0; i<nickSize; ++i) {
		QCOMPARE(parser.step, ActuatorConfigParserStep::NICK);
		QCOMPARE(parser.byteIndex, (uint16_t) i);
		parser.parse((uint8_t)c.nickName[i]);
	}
	QCOMPARE(parser.byteIndex, (uint16_t) 0);

	for(i=0; i<nickSize; ++i) {
		QCOMPARE(parser.config->nickName[i], c.nickName[i]);
	}

	if(c.hasGearRatio()) {
		// Send gear ratio numerator
		QCOMPARE(parser.step, ActuatorConfigParserStep::GEAR_NUMERATOR);
		parser.parse(c.gearRatioNumerator);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->gearRatioNumerator, c.gearRatioNumerator);

		// Send gear ratio denominator
		QCOMPARE(parser.step, ActuatorConfigParserStep::GEAR_DENOMINATOR);
		parser.parse(c.gearRatioDenominator);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->gearRatioDenominator, c.gearRatioDenominator);
	}

	if(c.hasPositionFeedback()) {
		// Send position feedback pin
		QCOMPARE(parser.step, ActuatorConfigParserStep::POSITION_FEEDBACK);
		parser.parse(c.positionFeedbackPin);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->positionFeedbackPin, c.positionFeedbackPin);
	}

	if(c.hasTachometer()) {
		// Send tachometer pin
		//qDebug()<<(uint16_t)parser.step<< (uint16_t)ServoConfigParser::TACHOMETER;
		QCOMPARE(parser.step, ActuatorConfigParserStep::TACHOMETER);
		parser.parse(c.tachometerPin);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->tachometerPin, c.tachometerPin);
	}


	if(c.hasIncrementalEncoder()) {
		// Send encoder pin A
		QCOMPARE(parser.step, ActuatorConfigParserStep::ENCODER_PIN_A);
		parser.parse(c.incrementalEncoderPinA);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->incrementalEncoderPinA, c.incrementalEncoderPinA);

		// Send encoder pin B
		QCOMPARE(parser.step, ActuatorConfigParserStep::ENCODER_PIN_B);
		parser.parse(c.incrementalEncoderPinB);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->incrementalEncoderPinB, c.incrementalEncoderPinB);

		// Send encoder debounce amount
		QCOMPARE(parser.step, ActuatorConfigParserStep::ENCODER_DEBOUNCE);
		parser.parse(c.incrementalencoderDebounceCount);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->incrementalencoderDebounceCount, c.incrementalencoderDebounceCount);
	}

	if(c.hasLimitSwitchStart()) {
		// Send limit switch start pin
		QCOMPARE(parser.step, ActuatorConfigParserStep::LIMIT_SWITCH_PIN_START);
		parser.parse(c.limitSwitchPinStart);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->limitSwitchPinStart, c.limitSwitchPinStart);
	}

	if(c.hasLimitSwitchEnd()) {
		// Send limit switch end pin
		QCOMPARE(parser.step, ActuatorConfigParserStep::LIMIT_SWITCH_PIN_END);
		parser.parse(c.limitSwitchPinEnd);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->limitSwitchPinEnd, c.limitSwitchPinEnd);
	}

	if(c.hasLimitSwitchStart() || c.hasLimitSwitchEnd()) {
		// Send limit switch debounce count
		QCOMPARE(parser.step, ActuatorConfigParserStep::LIMIT_SWITCH_DEBOUNCE);
		parser.parse(c.limitSwitchDebounceCount);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->limitSwitchDebounceCount, c.limitSwitchDebounceCount);
	}

	switch(parser.config->type) {
	case(ActuatorType::DC_MOTOR): {
	} break;
	case(ActuatorType::STEP_MOTOR): {
		// Send step motor phase count
		QCOMPARE(parser.step, ActuatorConfigParserStep::STEP_PHASE_COUNT);
		parser.parse(c.stepMotorPhaseCount);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->stepMotorPhaseCount, c.stepMotorPhaseCount);

		// Send step motor steps per rotation
		QCOMPARE(parser.step, ActuatorConfigParserStep::STEP_STEPS_PER_ROTATION);
		cv.uint16[0]=c.stepMotorStepsPerRotation;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::STEP_STEPS_PER_ROTATION);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->stepMotorStepsPerRotation, c.stepMotorStepsPerRotation);
	}
	break;
	case(ActuatorType::RC_SERVO): {
		// Send RC servo pwm pin
		QCOMPARE(parser.step, ActuatorConfigParserStep::RC_SERVO_PIN);
		parser.parse(c.rcServoPin);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rcServoPin, c.rcServoPin);
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


	//qDebug()<<"STEP "<< parser.step << ServoConfigParser::RANGE_START;
	// Send range start
	switch(parser.config->representation) {
	case(ActuatorValueRepresentation::BIT): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		parser.parse(c.rangeStart.bit?(uint8_t)1:(uint8_t)0);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.bit, c.rangeStart.bit);
	}
	break;
	case(ActuatorValueRepresentation::BYTE): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		parser.parse(c.rangeStart.byte);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.byte, c.rangeStart.byte);
	}
	break;
	case(ActuatorValueRepresentation::WORD): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		cv.uint16[0]=c.rangeStart.word;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.word, c.rangeStart.word);
	}
	break;
	case(ActuatorValueRepresentation::DOUBLE_WORD): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		cv.uint32[0]=c.rangeStart.doubleWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.doubleWord, c.rangeStart.doubleWord);
	}
	break;
	case(ActuatorValueRepresentation::QUAD_WORD): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		cv.uint64=c.rangeStart.quadWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.quadWord, c.rangeStart.quadWord);
	}
	break;
	case(ActuatorValueRepresentation::SINGLE_FLOAT): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		cv.float32[0]=c.rangeStart.singlePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.singlePrecision, c.rangeStart.singlePrecision);
	}
	break;
	case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_START);
		cv.float64=c.rangeStart.doublePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_START);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeStart.doublePrecision, c.rangeStart.doublePrecision);
	}
	break;
	default:
	case(ActuatorValueRepresentation::REPRESENTATION_COUNT): {
		QFAIL("bad representation for range start");
	}
	break;
	}


	// Send range span
	switch(parser.config->representation) {
	case(ActuatorValueRepresentation::BIT): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(c.rangeSpan.bit?(uint8_t)1:(uint8_t)0);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.bit, c.rangeSpan.bit);
	}
	break;
	case(ActuatorValueRepresentation::BYTE): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(c.rangeSpan.byte);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.byte, c.rangeSpan.byte);
	}
	break;
	case(ActuatorValueRepresentation::WORD): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		cv.uint16[0]=c.rangeSpan.word;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.word, c.rangeSpan.word);
	}
	break;
	case(ActuatorValueRepresentation::DOUBLE_WORD): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		cv.uint32[0]=c.rangeSpan.doubleWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.doubleWord, c.rangeSpan.doubleWord);
	}
	break;
	case(ActuatorValueRepresentation::QUAD_WORD): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		cv.uint64=c.rangeSpan.quadWord;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.quadWord, c.rangeSpan.quadWord);
	}
	break;
	case(ActuatorValueRepresentation::SINGLE_FLOAT): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		cv.float32[0]=c.rangeSpan.singlePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.singlePrecision, c.rangeSpan.singlePrecision);
	}
	break;
	case(ActuatorValueRepresentation::DOUBLE_FLOAT): {
		QCOMPARE(parser.step,  ActuatorConfigParserStep::RANGE_SPAN);
		cv.float64=c.rangeSpan.doublePrecision;
		parser.parse(cv.uint8[0]);
		QCOMPARE(parser.byteIndex, (uint16_t) 1);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[1]);
		QCOMPARE(parser.byteIndex, (uint16_t) 2);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[2]);
		QCOMPARE(parser.byteIndex, (uint16_t) 3);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[3]);
		QCOMPARE(parser.byteIndex, (uint16_t) 4);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[4]);
		QCOMPARE(parser.byteIndex, (uint16_t) 5);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[5]);
		QCOMPARE(parser.byteIndex, (uint16_t) 6);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[6]);
		QCOMPARE(parser.byteIndex, (uint16_t) 7);
		QCOMPARE(parser.step, ActuatorConfigParserStep::RANGE_SPAN);
		parser.parse(cv.uint8[7]);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.config->rangeSpan.doublePrecision, c.rangeSpan.doublePrecision);
	}
	break;
	default:
	case(ActuatorValueRepresentation::REPRESENTATION_COUNT): {
		QFAIL("bad representation for range span");
	}
	break;
	}

}

void TestArduMY::singleActuatorConfigSerializerRun(ActuatorConfig &c) const
{
	/*
	c.clear();
	c.flags=0xFFFF;
	c.type=ActuatorType::DC_MOTOR;

	c.gearRatioDenominator=20;
	c.gearRatioNumerator=21;
	c.incrementalencoderDebounceCount=22;
	c.incrementalEncoderPinA=23;
	c.incrementalEncoderPinB=24;
	c.limitSwitchDebounceCount=25;
	c.limitSwitchPinStart=26;
	c.limitSwitchPinEnd=27;
	c.positionFeedbackPin=28;
	//c.stepMotorPhaseCount=29;	c.stepMotorStepsPerRotation=30;
	*/
	ActuatorConfigSerializer serializer;
	serializer.setConfig(c);
	ActuatorConfigParser parser;
	ActuatorConfig to;
	to.clear();
	parser.setConfig(to);

	while(serializer.hasMoreData()) {
		uint8_t byte=serializer.nextByte();
		parser.parse(byte);
	}

	QCOMPARE(c.flags, to.flags);
	QCOMPARE(c.type, to.type);
	QCOMPARE(c.representation, to.representation);
	//qDebug()<<"NICK FROM "<<QString::fromLocal8Bit((char *)c.nickName, 20)<<" TO "<<QString::fromLocal8Bit((char *)to.nickName, 20);
	for(int i=0; i<20; ++i) {
		QCOMPARE(c.nickName[i], to.nickName[i]);
	}
	if(c.hasGearRatio()) {
		QCOMPARE(c.gearRatioNumerator, to.gearRatioNumerator);
		QCOMPARE(c.gearRatioDenominator, to.gearRatioDenominator);
	}
	if(c.hasPositionFeedback()) {
		QCOMPARE(c.positionFeedbackPin, to.positionFeedbackPin);
	}
	if(c.hasTachometer()) {
		QCOMPARE(c.tachometerPin, to.tachometerPin);
	}
	if(c.hasIncrementalEncoder()) {
		QCOMPARE(c.incrementalEncoderPinA, to.incrementalEncoderPinA);
		QCOMPARE(c.incrementalEncoderPinB, to.incrementalEncoderPinB);
		QCOMPARE(c.incrementalencoderDebounceCount, to.incrementalencoderDebounceCount);
	}
	if(c.hasLimitSwitchStart()) {
		QCOMPARE(c.limitSwitchPinStart, to.limitSwitchPinStart);
	}
	if(c.hasLimitSwitchEnd()) {
		QCOMPARE(c.limitSwitchPinEnd, to.limitSwitchPinEnd);
	}
	if(c.hasLimitSwitchStart() || c.hasLimitSwitchEnd()) {
		QCOMPARE(c.limitSwitchDebounceCount, to.limitSwitchDebounceCount);
	}
	if(STEP_MOTOR==c.type) {
		QCOMPARE(c.stepMotorPhaseCount, to.stepMotorPhaseCount);
		QCOMPARE(c.stepMotorStepsPerRotation, to.stepMotorStepsPerRotation);

	} else if(RC_SERVO==c.type) {
		QCOMPARE(c.rcServoPin, to.rcServoPin);
	}
	QCOMPARE(c.rangeStart.quadWord, to.rangeStart.quadWord);
	QCOMPARE(c.rangeSpan.quadWord, to.rangeSpan.quadWord);

	// Finally check that our findings match the result from the official "equals" overload

	QVERIFY(c == to);
}

void TestArduMY::testActuatorConfigParser()
{
	// Generate random configurations and feed them through a parser to verify that it works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Parser Round "<<j<<" / 100";
		for(int i=0; i<1000; ++i) {
			ActuatorConfig c=randomConfig();
			singleActuatorConfigParserRun(c);
		}
	}

}


void TestArduMY::testActuatorConfigSerializer()
{
	// Generate random configurations and feed them through a serializer to verify that it works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Serializer Round "<<j<<" / 100";
		for(int i=0; i<1000; ++i) {
			ActuatorConfig c=randomConfig();
			singleActuatorConfigSerializerRun(c);
		}
	}
}

void TestArduMY::testActuatorValueParser()
{
	// ActuatorValueParser parser;
}



void TestArduMY::testCommandParser()
{

	/*
	// Conduct sync
	for(uint8_t i=0; i<4; ++i) {
		QCOMPARE(commandParser.currentCommand, OCTOMY_SYNC);
		QCOMPARE(commandParser.magicDetector.idx, i);
		parser.parse(commandParser.magic[i]);
	}

	// Verify that sync completed correctly
	QCOMPARE(commandParser.currentCommand, OCTOMY_AWAITING_COMMAND);

	// Start the actuator config command
	ParserState parserState=OCTOMY_SET_ACTUATOR_CONFIG;
	parser.parse((uint8_t)parserState);
	QCOMPARE(commandParser.currentCommand, parserState);

	// Send actuator index
	static const uint8_t actuatorIndex=12;
	parser.parse(actuatorIndex);

	QCOMPARE(parser.byteIndex, (uint16_t) 0);

	*/

}


QTEST_MAIN(TestArduMY)
