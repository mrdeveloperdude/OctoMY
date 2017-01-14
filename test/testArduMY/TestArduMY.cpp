#include "TestArduMY.hpp"
#include "utility/Utility.hpp"

#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"


#include "../arduino/ActuatorValueParser.hpp"
#include "../arduino/ActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"

#include <QDebug>
#include <QtGlobal>

#define LOOPS (10)

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
	float v=0.0/0.0;
	while(qIsNaN(v)) {
		v=qrand();
		v/=RAND_MAX;
		float v2=qrand()-qrand();
		v*=v2;
	}
	return v;
}

static double dfrand()
{
	double v=0.0/0.0;
	while(qIsNaN(v)) {
		v=qrand();
		v/=RAND_MAX;
		double v2=qrand()-qrand();
		v*=v2;
	}
	return v;
}


static void randomValue(ActuatorValue &v, ActuatorValueRepresentation rep)
{
	v.quadWord=0x0;
	switch(rep) {
	case(BIT):
		v.bit=percentChance(50);
		break;
	case(BYTE):
		v.byte=qrand()%0xFF;
		break;
	case(WORD):
		v.doubleWord=qrand()%0xFFFF;
		break;
	case(DOUBLE_WORD):
		v.doubleWord=qrand()%0xFFFFFFFF;
		break;
	default:// Just make sure its random m-kay?
	case(REPRESENTATION_COUNT):
	case(QUAD_WORD): {
		uint64_t qw=(qrand()%0xFFFFFFFF);
		qw<<=32;
		qw|=(qrand()%0xFFFFFFFF);
		v.quadWord=qw;
	}
	break;
	case(SINGLE_FLOAT):
		v.singlePrecision=frand();
		break;
	case(DOUBLE_FLOAT):
		v.doublePrecision=dfrand();
		break;
	}
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

	//do {		c.representation=(ActuatorValueRepresentation)(qrand() % ((quint8)ActuatorValueRepresentation::REPRESENTATION_COUNT)) ;	} while (ActuatorValueRepresentation::BIT==c.representation);

	//c.representation=ActuatorValueRepresentation::WORD;// All the same for laughs

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


ActuatorState TestArduMY::randomState(const ActuatorConfig &c) const
{
	ActuatorState s;
	s.flags=0x00;
	s.setDirty(percentChance(50));
	s.setLimp(percentChance(50));
	randomValue(s.value, c.representation);
	return s;
}


Actuator TestArduMY::randomActuator() const
{
	Actuator a;
	a.config=randomConfig();
	a.state=randomState(a.config);
	return a;
}





ActuatorSet TestArduMY::randomActuatorSet()
{
	ActuatorSet set;
	const auto setSize=qrand()%0xFF;
	set.setSize(setSize);
	for(auto i=0; i<setSize; ++i) {
		set[i]=randomActuator();
	}
	return set;
}




template <typename T>
union Fuzzer {
	T object;
	uint8_t bytes[sizeof(T)];
	explicit Fuzzer() {
		for(size_t i=0; i<sizeof(bytes); ++i)	{
			bytes[i]=qrand();
		}
	}

};

ActuatorConfig TestArduMY::fuzzConfig() const
{
	Fuzzer<ActuatorConfig> fuzzer;
	// Return resulting fuzzed config
	return fuzzer.object;
}



ActuatorState TestArduMY::fuzzState() const
{
	Fuzzer<ActuatorState> fuzzer;
	// Return resulting fuzzed state
	return fuzzer.object;
}




Actuator TestArduMY::fuzzActuator() const
{
	Fuzzer<Actuator> fuzzer;
	// Return resulting fuzzed actuator
	return fuzzer.object;
}



ActuatorSet TestArduMY::fuzzActuatorSet()
{
	ActuatorSet set;
	const auto setSize=qrand()%0xFF;
	set.setSize(setSize);
	for(auto i=0; i<setSize; ++i) {
		set[i]=fuzzActuator();
	}
	return set;
}





////////////////////////////////////////////////////////////////////////////////

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




void TestArduMY::singleActuatorConfigEqualsRun(const ActuatorConfig &a) const
{
	QCOMPARE(a,a);
	QVERIFY(a.isEqual(a));
	QVERIFY(a==a);
	for(int i=0; i<8; ++i) {
		ActuatorConfig b=a;
		QCOMPARE(a,b);
		QVERIFY(a.isEqual(b));
		QVERIFY(b.isEqual(a));
		QVERIFY(a==b);

		switch(i) {
		case(0): {
			b.setAbsoluteEncoder(!a.hasAbsoluteEncoder());
		}
		break;
		case(1): {
			b.setContinuous(!a.isContinuous());
		}
		break;
		case(2): {
			//b.setDirty(!a.isDirty());
			b.setGearRatio(!a.hasGearRatio());
		}
		break;
		case(3): {
			b.setIncrementalEncoder(!a.hasIncrementalEncoder());
		}
		break;
		case(4): {
			b.setLimitSwitchEnd(!a.hasLimitSwitchEnd());
		}
		break;
		case(5): {
			b.setLimitSwitchStart(!a.hasLimitSwitchStart());
		}
		break;
		case(6): {
			b.setLinear(!a.isLinear());
		}
		break;
		case(7): {
			b.setPositionFeedback(!a.hasPositionFeedback());
		}
		break;
		case(8): {
			b.setTachometer(!a.hasTachometer());
		}
		break;
			//TODO: Add cases to exercise the rest of equals code (not just for flags).
		}
		QVERIFY(!a.isEqual(b));
		QVERIFY(!b.isEqual(a));
		QVERIFY(a!=b);
	}

}

static QString byteToStr(uint8_t byte)
{
	return QString("%1").arg(byte, 8, 2, QLatin1Char('0'))+" ( 0x"+QString("%1").arg(byte, 2, 16, QLatin1Char('0'))+","+ QString("%1").arg(byte, 3, 10, QLatin1Char(' '))+ " )";
}

void TestArduMY::singleActuatorSetParserRun(ActuatorSet &inSet) const
{
	//qDebug()<<"-------------------";
	ActuatorSet outSet;
	const auto inSize=inSet.size();
	//The out set must match in size as this would be carried out by the set size command
	outSet.setSize(inSize);
	//We also copy the representations of the sets as that would have been carried out in a series of set config commands
	for(size_t i =0; i<inSize; ++i) {
		outSet[i].config.representation=inSet[i].config.representation;
	}
	Converter cv;
	ActuatorValueParser parser;

	// Check that it was default-initialized properly
	QCOMPARE(parser.set, (ActuatorSet *)nullptr);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::END_OF_OP);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)0);

	parser.setSet(outSet);

	// Check that the actuator set was received properly
	QCOMPARE(parser.set, &outSet);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ENABLED_ACTUATOR_BITS);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)0);

	// Reset enable bits to all off
	const size_t enableBitsSize=sizeof(parser.enableBits);
	uint8_t enableBits[enableBitsSize]= {0};
	for(size_t i=0; i<enableBitsSize; ++i) {
		enableBits[i]=0x0;
	}
	// Feed in random enable bit sequence while counting on-bits
	uint8_t enabledCount=0;
	const uint8_t enableBitsActualSize = ( inSize + 7 ) / 8;
	for( size_t i = 0; i < enableBitsActualSize ; ++i ) {
		const uint8_t bits = ( qrand() % 0xFF );
		for( size_t j = 0 ; j < 8 ; ++j ) {
			// Make sure to not enable bits outside the number of actuators in the set!
			if( ( i * 8 + j ) >= inSize ) {
				break;
			}
			const uint8_t mask = ( 1 << j );
			const uint8_t value = ( mask & bits );
			if( 0 != value ) {
				enableBits[i] |= value;
				enabledCount++;
			}
		}
		//qDebug()<<"RANDOM BITS: " << byteToStr(bits) << " ACTUALLY WRITTEN: " << byteToStr(enableBits[i]);
	}
	//qDebug()<<"ACTUATOR COUNT: " << inSize << " ENABLE BITS BYTE COUNT: " << enableBitsActualSize << " ENABLED COUNT: " << enabledCount;
	// Parse enable bits
	for(size_t i=0; i<enableBitsActualSize; ++i) {
		QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ENABLED_ACTUATOR_BITS);
		QCOMPARE(parser.byteIndex, (uint16_t)i);
		parser.parse(enableBits[i]);
	}
	// Check that enabled bits were transferred correctly
	for(size_t i=0; i<enableBitsSize; ++i) {
		if(parser.enableBits[i]!=enableBits[i] ) {
			qDebug()<<"ENABLE-BIT MISMATCH DETECTED FOR BYTE "<< QString("%1").arg(i+1, 2, 10, QLatin1Char(' '))<<"/"<<enableBitsSize<<"  PARSER: "<<byteToStr(parser.enableBits[i])<<" vs. ORIGINAL: "<<byteToStr(enableBits[i]);
		}
		QCOMPARE(parser.enableBits[i], enableBits[i]);
	}
	// Check that parse step was advanced properly
	if(ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES != parser.step) {
		/*
				for(size_t i=0; i<enableBitsSize; ++i) {
					if(parser.enableBits[i]!=enableBits[i] ) {
						qDebug()<<"ENABLE-BIT MISMATCH DETECTED FOR BYTE "<< QString("%1").arg(i+1, 2, 10, QLatin1Char(' '))<<"/"<<enableBitsSize<<"  PARSER: "<<byteToStr(parser.enableBits[i])<<" vs. ORIGINAL: "<<byteToStr(enableBits[i]);
					}
					*/
		qDebug()<<"BOB";
	}
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);


	// Send values and check that they were sucessfully transferred per batch
	QString acc;
	uint8_t bitCount=0;
	uint8_t bits=0x00;

	///// BIT ACTUATOR VALUES
	////////////////////////////
	// Bit actuator values are accumulated into bytes before sending
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::BIT==a.config.representation) {
				if(a.state.value.bit) {
					bits |= ( 1 << bitCount );
				}
				bitCount++;
				if( bitCount >= 8 ) {
					acc += byteToStr(bits)+" ";
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					parser.parse(bits);
					bitCount=0;
					bits=0x00;
				}
			}
		}
	}
	// Send any remaining bits we have
	if(bitCount>0) {
		//acc+=" REST: "+byteToStr(bits);
		QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::BIT);
		parser.parse(bits);
		bitCount=0;
		bits=0;
	}

	//qDebug()<<"ACCUMULATED BIT VALUES SENT:       "<<acc;

	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::BIT == a.config.representation) {
				//qDebug()<<"SENT BIT VALUE FOR "<<byte<<"."<<bit<<": " << a.state.value.bit;
				Actuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.bit, b.state.value.bit);
			}
		}
	}

	///// BYTE ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::BYTE==a.config.representation) {
				//qDebug().noquote().nospace()<<"SENDING BYTE VALUE FOR "<<byte<<"."<<bit<<" (" << QString("%1").arg(i,2,10,QChar(' '))<< "/" << QString("%1").arg(inSize,2,10,QChar(' '))<< "): "<<a.state.value.byte;
				QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
				QCOMPARE(parser.currentActuatorIndex, (int16_t )i);
				Actuator &b=outSet[i];
				QCOMPARE((uint8_t)b.config.representation, (uint8_t)a.config.representation);
				parser.parse(a.state.value.byte);
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.byte, b.state.value.byte);
			}
		}
	}

	///// WORD ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::WORD==a.config.representation) {
				//qDebug()<<"SENDING WORD VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.word;
				cv.int64=0x00;
				cv.uint16[0]=a.state.value.word;
				for(int j=0; j<2; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					parser.parse(cv.uint8[j]);
				}
				Actuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.word, b.state.value.word);
			}
		}
	}

	///// DWORD ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::DOUBLE_WORD==a.config.representation) {
				//qDebug()<<"SENDING DWORD VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.doubleWord;
				cv.int64=0x00;
				cv.uint32[0]=a.state.value.doubleWord;
				for(int j=0; j<4; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					parser.parse(cv.uint8[j]);
				}
				Actuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.doubleWord, b.state.value.doubleWord);
			}
		}
	}

	///// QWORD ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::QUAD_WORD==a.config.representation) {
				//qDebug()<<"SENDING QWORD VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.quadWord;
				cv.int64=0x00;
				cv.uint64=a.state.value.quadWord;
				for(int j=0; j<8; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					parser.parse(cv.uint8[j]);
				}
				Actuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.quadWord, b.state.value.quadWord);
			}
		}
	}

	///// SINGLE ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::SINGLE_FLOAT==a.config.representation) {
				//qDebug()<<"SENDING FLOAT VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.singlePrecision;
				cv.int64=0x00;
				cv.float32[0]=a.state.value.singlePrecision;
				for(int j=0; j<4; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					parser.parse(cv.uint8[j]);
				}
				Actuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.singlePrecision, b.state.value.singlePrecision);
			}
		}
	}

	///// DOUBLE ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			Actuator &a=inSet[i];
			if(ActuatorValueRepresentation::DOUBLE_FLOAT==a.config.representation) {
				//qDebug()<<"SENDING DOUBLE VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.doublePrecision;
				cv.int64=0x00;
				cv.float64=a.state.value.doublePrecision;
				for(int j=0; j<8; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					parser.parse(cv.uint8[j]);
				}
				Actuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.doublePrecision, b.state.value.doublePrecision);
			}
		}
	}
}





void TestArduMY::singleActuatorSetSerializerRun(ActuatorSet &inSet) const
{
	ActuatorValueSerializer serializer;
	serializer.setSet(inSet);
	ActuatorSet outSet;

	const auto inSize=inSet.size();
	// The out set must match in size as this would be carried out by the set size command
	outSet.setSize(inSize);
	// We also copy the representations of the sets as that would have been carried out in a series of set config commands
	for(size_t i =0; i<inSize; ++i) {
		outSet[i].config.representation=inSet[i].config.representation;
	}
	//Converter cv;

	ActuatorValueParser parser;
	parser.setSet(outSet);

	while(serializer.hasMoreData()) {
		uint8_t byte=serializer.nextByte();
		parser.parse(byte);
	}


	QCOMPARE(inSet,outSet);

}




////////////////////////////////////////////////////////////////////////////////

void TestArduMY::testMagicDetector()
{
	for(int k=0; k<100; ++k) {
		qDebug()<<"Magic Detector Round "<<k<<" / 100";
		const size_t magicSize=1+(qrand()%100);
		uint8_t magic[magicSize];
		MagicDetector magicDetector(magic,magicSize);
		for(int i=0; i<LOOPS; ++i) {
			// Generate random sequence
			const size_t testSequenceSize=magicSize+1+(qrand()%1000);
			uint8_t testSequence[testSequenceSize];
			for(size_t j=0; j<testSequenceSize; ++j) {
				uint8_t byte=qrand()%0xFF;
				// Ensure that the sequence does not contain magic by coincidence
				if(byte==magic[magicSize-1]) {
					byte^=1;
				}
				testSequence[j]=byte;
			}
			// Perform negative test
			{
				magicDetector.reset();
				uint32_t ct=0;
				for(size_t j=0; j<testSequenceSize; ++j) {
					const uint8_t in=testSequence[j];
					if(magicDetector.detect(in)) {
						break;
					}
					ct++;
				}
				QCOMPARE((uint32_t)ct, (uint32_t)(testSequenceSize));
			}
			// Insert the magic sequence
			const size_t insertPoint=qrand()%(testSequenceSize - magicSize);
			for(size_t l=0; l<magicSize; ++l) {
				testSequence[insertPoint + l]=magic[l];
			}
			// Perform positive test
			{
				magicDetector.reset();
				uint32_t ct=0;
				for(size_t j=0; j<testSequenceSize; ++j) {
					const uint8_t in=testSequence[j];
					ct++;
					if(magicDetector.detect(in)) {
						QCOMPARE(ct, (uint32_t)(insertPoint+magicSize));
						break;
					}
				}
			}
		}
	}
}



void TestArduMY::testActuatorRandomConfigParser()
{
	// Generate random configurations and feed them through a parser to verify that it works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Random Config Parser Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorConfig c=randomConfig();
			singleActuatorConfigParserRun(c);
		}
	}
}


void TestArduMY::testActuatorRandomConfigSerializer()
{
	// Generate random configurations and feed them through a serializer to verify that it works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Random Config Serializer Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorConfig c=randomConfig();
			singleActuatorConfigSerializerRun(c);
		}
	}
}


void TestArduMY::testActuatorRandomConfigEquals()
{
	// Generate random configurations and feed them through an equality check to verify the equality comparison code
	for(int j=0; j<100; ++j) {
		qDebug()<<"Random Config Equals Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			const ActuatorConfig a=randomConfig();
			singleActuatorConfigEqualsRun(a);
		}
	}
}


void TestArduMY::testActuatorRandomValueParser()
{
	// Generate random configurations and random values to match, then feed them through a parser to verify that the parsing works as it should
	for(int j=0; j<100; ++j) {
		qDebug()<<"Random Values Parser Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorSet set=randomActuatorSet();
			// Communication is impossible when set size is 0, so we skip those
			if(set.size()>0) {
				singleActuatorSetParserRun(set);
			}
		}
	}
}


void TestArduMY::testActuatorRandomValueSerializer()
{
	// Generate random configurations and random values to match, then feed them through a serializer to verify that it works as it should
	for(int j=0; j<100; ++j) {
		qDebug()<<"Random Values Serializer Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorSet set=randomActuatorSet();
			// Communication is impossible when set size is 0, so we skip those
			if(set.size()>0) {
				singleActuatorSetSerializerRun(set);
			}
		}
	}
}

void TestArduMY::testActuatorFuzzConfigSerializer()
{
	// Generate fuzz configurations and feed them through a serializer to verify that it works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Fuzzed Config Serializer Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorConfig c=fuzzConfig();
			singleActuatorConfigSerializerRun(c);
		}
	}
}


void TestArduMY::testActuatorFuzzConfigParser()
{
	// Generate fuzz configurations and feed them through a parser to verify that it works as it should.
	for(int j=0; j<100; ++j) {
		qDebug()<<"Fuzzed Config Parser Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorConfig c=fuzzConfig();
			singleActuatorConfigParserRun(c);
		}
	}
}


void TestArduMY::testActuatorFuzzConfigEquals()
{
	// Generate fuzz configurations and feed them through an equality check to verify the equality comparison code
	for(int j=0; j<100; ++j) {
		qDebug()<<"Fuzzed Config Equals Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			const ActuatorConfig a=fuzzConfig();
			singleActuatorConfigEqualsRun(a);
		}
	}
}

void TestArduMY::testActuatorFuzzValueParser()
{
	// Generate fuzz configurations and fuzz values to match, then feed them through a parser to verify that the parsing works as it should
	for(int j=0; j<100; ++j) {
		qDebug()<<"Fuzzed Values Parser Round "<<j<<" / 100";
		for(int i=0; i<LOOPS; ++i) {
			ActuatorSet set=fuzzActuatorSet();
			singleActuatorSetParserRun(set);
		}
	}
}


void TestArduMY::testRepresentationBoundary()
{
	ActuatorSet inSet;
	ActuatorSet outSet;
	inSet.setSize(4);
	outSet.setSize(4);
	inSet[0].config.representation=ActuatorValueRepresentation::BYTE;
	inSet[0].state.value.byte=111;
	inSet[1].config.representation=ActuatorValueRepresentation::BYTE;
	inSet[1].state.value.byte=222;
	inSet[2].config.representation=ActuatorValueRepresentation::WORD;
	inSet[2].state.value.word=33333;
	inSet[3].config.representation=ActuatorValueRepresentation::WORD;
	inSet[3].state.value.word=44444;
	for(size_t i =0; i<4; ++i) {
		outSet[i].config.representation=inSet[i].config.representation;
		outSet[i].state.value.quadWord=0;
	}
	ActuatorValueParser parser;

	QCOMPARE(parser.set, (ActuatorSet *)nullptr);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::END_OF_OP);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)0);

	parser.setSet(outSet);

	// Check that the actuator set was received properly
	QCOMPARE(parser.set, &outSet);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ENABLED_ACTUATOR_BITS);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	for(int i=0; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)0);
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::BIT);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)0);

	//All enabled
	const uint8_t enabled=1|2|4|8;
	parser.parse(enabled);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}

	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::BYTE);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)0);

	parser.parse(inSet[0].state.value.byte);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::BYTE);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)1);

	parser.parse(inSet[1].state.value.byte);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::WORD);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)2);


	Converter cv;
	cv.int16[0]=inSet[2].state.value.word;
	parser.parse(cv.int8[0]);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)1);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::WORD);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)2);

	parser.parse(cv.int8[1]);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::WORD);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)3);

	cv.int16[0]=inSet[3].state.value.word;
	parser.parse(cv.int8[0]);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)1);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::WORD);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)3);

	parser.parse(cv.int8[1]);

	QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
	QCOMPARE(parser.enableBits[0], enabled);
	for(int i=1; i<32; ++i) {
		QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
	}
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ActuatorValueRepresentation::REPRESENTATION_COUNT);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)0);

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
