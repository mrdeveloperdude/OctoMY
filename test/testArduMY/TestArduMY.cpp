#include "TestArduMY.hpp"
#include "utility/Utility.hpp"

#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"


#include "../arduino/ActuatorValueParser.hpp"
#include "../arduino/ActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QDebug>
#include <QtGlobal>

#define LOOPS (10)

#define VERBOSE_TEST (false)

#define DO_LOOPS_START(NAME, LOOPS) \
const int step=(LOOPS/100);\
int counter=0; \
for(int lc=0; lc<LOOPS; ++lc) { \
	counter++; \
	if(counter>step) { \
		counter=0; \
		if(VERBOSE_TEST){qDebug()<<NAME<<((lc*100)/LOOPS)<<" / 100";} \
	}

#define DO_LOOPS_END \
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



static QString byteToStr(uint8_t byte)
{
	return QString("%1").arg(byte, 8, 2, QLatin1Char('0'))+" ( 0x"+QString("%1").arg(byte, 2, 16, QLatin1Char('0'))+","+ QString("%1").arg(byte, 3, 10, QLatin1Char(' '))+ " )";
}



static QString valueToString(const ActuatorValue &v, const ActuatorValueRepresentation &rep)
{
	QString ret;
	switch(rep) {
	case(BIT):
		ret=QString("BIT(")+ (v.bit?"TRUE":"FALSE")+")";
		break;
	case(BYTE):
		ret="BYTE("+ QString::number(v.byte)+")";
		break;
	case(WORD):
		ret="WORD("+ QString::number(v.word)+")";
		break;
	case(DOUBLE_WORD):
		ret="DWORD("+ QString::number(v.doubleWord)+")";
		break;
	default:// Just make sure its random m-kay?
	case(REPRESENTATION_COUNT):
		ret="OUT_OF_RANGE!";
		break;
	case(QUAD_WORD): {
		ret="QWORD("+ QString::number(v.quadWord)+")";
	}
	break;
	case(SINGLE_FLOAT):
		ret="FLOAT("+ QString::number(v.singlePrecision)+")";
		break;
	case(DOUBLE_FLOAT):
		ret="DOUBLE("+ QString::number(v.doublePrecision)+")";
		break;
	}
	return ret;
}


////////////////////////////////////////////////////////////////////////////////





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
	if((uint32_t)set.size() != (uint32_t)setSize) {
		qWarning()<<"ERROR: set size was incorrect";
	}

	for(auto i=0; i<setSize; ++i) {
		Actuator a=randomActuator();
		Actuator &b=set[i];
		b=a;
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

void TestArduMY::testMagicDetector()
{
	DO_LOOPS_START("MagicDetector", LOOPS)

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
	DO_LOOPS_END
}

void TestArduMY::testActuatorValue()
{
	DO_LOOPS_START("ActuatorValue", LOOPS)
	for(int i=0; i<(int)ActuatorValueRepresentation::REPRESENTATION_COUNT; ++i) {
		ActuatorValueRepresentation rep=(ActuatorValueRepresentation)i;
		ActuatorValue a;
		QCOMPARE(a.bit,false);
		QCOMPARE(a.byte,(typeof(a.byte))0x00);
		QCOMPARE(a.word,(typeof(a.word))0x00);
		QCOMPARE(a.doubleWord,(typeof(a.doubleWord))0x00);
		QCOMPARE(a.singlePrecision,(typeof(a.singlePrecision))0.0f);
		QCOMPARE(a.doublePrecision,(typeof(a.doublePrecision))0.0f);
		randomValue(a,rep);
		ActuatorValue b;
		QCOMPARE(b.bit,false);
		QCOMPARE(b.byte,(typeof(b.byte))0x00);
		QCOMPARE(b.word,(typeof(b.word))0x00);
		QCOMPARE(b.doubleWord,(typeof(b.doubleWord))0x00);
		QCOMPARE(b.singlePrecision,(typeof(b.singlePrecision))0.0f);
		QCOMPARE(b.doublePrecision,(typeof(b.doublePrecision))0.0f);
		a=b;
		QVERIFY(a.isEqual(b,rep));
		QCOMPARE(a,b);
	}
	DO_LOOPS_END
}


void TestArduMY::testValueConverter()
{
	DO_LOOPS_START("Converter", LOOPS)
	for(int i=0; i<(int)ActuatorValueRepresentation::REPRESENTATION_COUNT; ++i) {
		ActuatorValueRepresentation rep=(ActuatorValueRepresentation)i;
		/*
		Converter a;
		QCOMPARE(a.bit,false);
		QCOMPARE(a.byte,(typeof(a.byte))0x00);
		QCOMPARE(a.word,(typeof(a.word))0x00);
		QCOMPARE(a.doubleWord,(typeof(a.doubleWord))0x00);
		QCOMPARE(a.singlePrecision,(typeof(a.singlePrecision))0.0f);
		QCOMPARE(a.doublePrecision,(typeof(a.doublePrecision))0.0f);
		randomValue(a,rep);
		ActuatorValue b;
		QCOMPARE(b.bit,false);
		QCOMPARE(b.byte,(typeof(b.byte))0x00);
		QCOMPARE(b.word,(typeof(b.word))0x00);
		QCOMPARE(b.doubleWord,(typeof(b.doubleWord))0x00);
		QCOMPARE(b.singlePrecision,(typeof(b.singlePrecision))0.0f);
		QCOMPARE(b.doublePrecision,(typeof(b.doublePrecision))0.0f);
		a=b;
		QVERIFY(a.isEqual(b,rep));
		QCOMPARE(a,b);
		*/
	}
	DO_LOOPS_END
}



void TestArduMY::testActuatorRandomConfigParser()
{
	DO_LOOPS_START("ActuatorConfigEquals Random", LOOPS)
	const ActuatorConfig a=randomConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END

}

void TestArduMY::testActuatorRandomConfigSerializer()
{
	DO_LOOPS_START("ActuatorConfigSerializer Random", LOOPS)
	ActuatorConfig c=randomConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigSerializer.inc.hpp"
	DO_LOOPS_END
}


void TestArduMY::testActuatorRandomConfigEquals()
{
	DO_LOOPS_START("ActuatorConfigEquals Random", LOOPS)
	const ActuatorConfig a=randomConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END
}




void TestArduMY::testActuatorRandomValueParser()
{
	DO_LOOPS_START("ActuatorValueParser Random", LOOPS)
	ActuatorSet inSet=randomActuatorSet();
	if(inSet.size()>0) {
#		define TEST_INCLUDED
#		include "testActuatorValueParser.inc.hpp"
	}
	DO_LOOPS_END
}




void TestArduMY::testActuatorRandomValueEquals()
{
	DO_LOOPS_START("ActuatorValueEquals Random", LOOPS)
	ActuatorSet a=randomActuatorSet();
#		define TEST_INCLUDED
#		include "testActuatorValueEquals.inc.hpp"
	DO_LOOPS_END
}


void TestArduMY::testActuatorRandomValueSerializer()
{
	DO_LOOPS_START("ActuatorValueSerializer Random", LOOPS)
	ActuatorSet inSet=randomActuatorSet();
	// Communication is impossible when set size is 0, so we skip those
	if(inSet.size()>0) {
#		define TEST_INCLUDED
#		include "testActuatorValueSerializer.inc.hpp"
	}
	DO_LOOPS_END
}



void TestArduMY::testActuatorFuzzConfigSerializer()
{
	DO_LOOPS_START("ActuatorConfigEquals Fuzz", LOOPS)
	const ActuatorConfig a=fuzzConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END
}


void TestArduMY::testActuatorFuzzConfigParser()
{
	DO_LOOPS_START("ActuatorConfigParser Fuzz", LOOPS)
	ActuatorConfig c=fuzzConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigParser.inc.hpp"
	DO_LOOPS_END
}


void TestArduMY::testActuatorFuzzConfigEquals()
{
	DO_LOOPS_START("ActuatorConfigEquals Fuzz", LOOPS)
	const ActuatorConfig a=fuzzConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END
}

void TestArduMY::testActuatorFuzzValueParser()
{
	DO_LOOPS_START("ActuatorValueParser Fuzz", LOOPS)
	ActuatorSet inSet=fuzzActuatorSet();
	if(inSet.size()>0) {
#		define TEST_INCLUDED
#		include "testActuatorValueParser.inc.hpp"
	}
	DO_LOOPS_END
}


void TestArduMY::testActuatorFuzzValueSerializer()
{
	DO_LOOPS_START("ActuatorValueSerializer Fuzz", LOOPS)
	ActuatorSet inSet=fuzzActuatorSet();
	// Communication is impossible when set size is 0, so we skip those
	if(inSet.size()>0) {
#		define TEST_INCLUDED
#		include "testActuatorValueSerializer.inc.hpp"
	}
	DO_LOOPS_END
}

void TestArduMY::testActuatorFuzzValueEquals()
{
	DO_LOOPS_START("ActuatorValueEquals Fuzz", LOOPS)
	ActuatorSet a=fuzzActuatorSet();
#		define TEST_INCLUDED
#		include "testActuatorValueEquals.inc.hpp"
	DO_LOOPS_END
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
	CommandParser parser;

	QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_SYNC);
	QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);
	QCOMPARE((int16_t)parser.actuators.size(), (int16_t) 0);


	// Conduct sync
	for(uint8_t i=0; i<4; ++i) {
		QCOMPARE((uint8_t)parser.currentCommand, (uint8_t)OCTOMY_SYNC);
		QCOMPARE(parser.magicDetector.idx, i);
		parser.parse(parser.magic[i]);
	}

	// Verify that sync completed correctly
	QCOMPARE((uint8_t)parser.currentCommand, (uint8_t)OCTOMY_AWAITING_COMMAND);
	QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);
	QCOMPARE((int16_t)parser.actuators.size(), (int16_t) 0);

	ParserState parserState=ParserState::OCTOMY_AWAITING_COMMAND;
	for(int i=0; i<10; ++i) {
		ActuatorSet set=randomActuatorSet();

		// Start the actuator count command
		parserState=OCTOMY_SET_ACTUATOR_COUNT;
		parser.parse((uint8_t)parserState);
		QCOMPARE(parser.currentCommand, parserState);
		QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);

		// Send actuator count
		parser.parse((uint8_t)set.size());

		// Verify that actuator count was received and processed correctly
		QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_AWAITING_COMMAND);
		QCOMPARE((int16_t)parser.actuators.size(), (int16_t) set.size());
		QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);


		for(uint8_t actuatorIndex=0; actuatorIndex<set.size(); ++actuatorIndex) {


			// Start the actuator config command
			parserState=OCTOMY_SET_ACTUATOR_CONFIG;
			parser.parse((uint8_t)parserState);
			QCOMPARE(parser.currentCommand, parserState);
			QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);

			// Send actuator index
			parser.parse(actuatorIndex);

			// Verify that actuator index was received and processed correctly
			QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_SET_ACTUATOR_CONFIG);
			QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) actuatorIndex);

			Actuator &a=set[actuatorIndex];
			ActuatorConfig &c=a.config;

			ActuatorConfigSerializer serializer;

			serializer.setConfig(c);

			while(serializer.hasMoreData()) {
				QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_SET_ACTUATOR_CONFIG);
				QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) actuatorIndex);
				parser.parse(serializer.nextByte());
			}

			// Verify that set config command completed and that parser is ready for new command
			QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_AWAITING_COMMAND);
			QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);



		}

		for(uint8_t i=0; i<10; ++i) {

			qDebug()<<"STEP: "<<i;
			// Change a random number of actuator values
			for(uint8_t actuatorIndex=0; actuatorIndex<set.size(); ++actuatorIndex) {
				Actuator &a=set[actuatorIndex];
				const bool dirty=percentChance(i*11);
				a.state.setDirty(dirty);
				randomValue(a.state.value, a.config.representation);
				qDebug()<<" ACTUATOR "<<actuatorIndex<<"/"<<set.size()<<" was set to "<<(dirty?"DIRTY":"CLEAN")<<" with value "<<valueToString(a.state.value,a.config.representation);
			}
			ActuatorValueSerializer serializer;
			serializer.setSet(set);

			// Start the actuator config command
			parserState=OCTOMY_SET_ACTUATOR_VALUES;
			parser.parse((uint8_t)parserState);
			QCOMPARE(parser.currentCommand, parserState);
			QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);

			while(serializer.hasMoreData()) {

				QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_SET_ACTUATOR_VALUES);
				QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);
				parser.parse(serializer.nextByte());
			}

			// Verify that set values command completed and that parser is ready for new command
			QCOMPARE((uint8_t)parser.currentCommand, (uint8_t) OCTOMY_AWAITING_COMMAND);
			QCOMPARE((int16_t)parser.actuatorConfigIndex, (int16_t) -1);
		}

		// Compare values at this point as they should all match
	}

}



void TestArduMY::testCommandSerializer()
{

}

QTEST_MAIN(TestArduMY)


































