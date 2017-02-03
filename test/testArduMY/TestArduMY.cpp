#include "TestArduMY.hpp"
#include "utility/Utility.hpp"

#include "../arduino/ArduMYActuatorConfigParser.hpp"
#include "../arduino/ArduMYActuatorConfigSerializer.hpp"


#include "../arduino/ArduMYActuatorValueParser.hpp"
#include "../arduino/ArduMYActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QDebug>
#include <QtGlobal>

#define LOOPS (100)

#define VERBOSE_TEST (false)

#define DO_LOOPS_START(NAME, LOOPS) \
const int step=(LOOPS/100);\
int counter=0; \
for(int lc=0; lc<LOOPS; ++lc) { \
	counter++; \
	if(counter>step) { \
		counter=0; \
		if(VERBOSE_TEST){qDebug().noquote().nospace()<<NAME<<": "<<((lc*100)/LOOPS)<<" / 100          ";} \
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
	return QString("%1").arg(byte, 8, 2, QLatin1Char('0'))+QString(" ( 0x%1").arg(byte, 2, 16, QLatin1Char('0'))+ QString(", %1").arg(byte, 3, 10, QLatin1Char(' '))+ QStringLiteral(" )");
}




QString actuatorValueRepresentationToString(const ArduMYActuatorValueRepresentation &rep)
{
	QString ret;
	switch(rep) {
	case(BIT):
		ret=QString("BIT ");
		break;
	case(BYTE):
		ret="BYTE";
		break;
	case(WORD):
		ret="WORD";
		break;
	case(DOUBLE_WORD):
		ret="DWRD";
		break;
	default:
	case(REPRESENTATION_COUNT):
		ret="ERR!";
		break;
	case(QUAD_WORD): {
		ret="QWRD";
	}
	break;
	case(SINGLE_FLOAT):
		ret="FLOT";
		break;
	case(DOUBLE_FLOAT):
		ret="DOUB";
		break;
	}
	return ret;
}

QString valueToString(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep)
{
	QString ret=actuatorValueRepresentationToString(rep)+" ( ";
	switch(rep) {
	case(BIT):
		ret+=(v.bit?"TRUE":"FALSE");
		break;
	case(BYTE):
		ret+=QString::number(v.byte);
		break;
	case(WORD):
		ret+=QString::number(v.word);
		break;
	case(DOUBLE_WORD):
		ret+=QString::number(v.doubleWord);
		break;
	default:
	case(REPRESENTATION_COUNT):
		ret+="OUT_OF_RANGE!";
		break;
	case(QUAD_WORD): {
		ret+=QString::number(v.quadWord);
	}
	break;
	case(SINGLE_FLOAT):
		ret+=QString::number(v.singlePrecision);
		break;
	case(DOUBLE_FLOAT):
		ret+=QString::number(v.doublePrecision);
		break;
	}
	return ret+" )";
}


QString actuatorConfigFlagsToString(const ArduMYActuatorConfig &c)
{
	QStringList list;
	list<< (c.isDirty()?              QStringLiteral("dirty"): QStringLiteral("-----"));
	list<< (c.isContinuous()?         QStringLiteral("contn"): QStringLiteral("-----"));
	list<< (c.isLinear()?             QStringLiteral("liner"): QStringLiteral("-----"));
	list<< (c.hasGearRatio()?         QStringLiteral("gearr"): QStringLiteral("-----"));
	list<< (c.hasAbsoluteEncoder()?   QStringLiteral("abenc"): QStringLiteral("-----"));
	list<< (c.hasIncrementalEncoder()?QStringLiteral("inenc"): QStringLiteral("-----"));
	list<< (c.hasPositionFeedback()?  QStringLiteral("posfb"): QStringLiteral("-----"));
	list<< (c.hasTachometer()?        QStringLiteral("tacho"): QStringLiteral("-----"));
	list<< (c.hasLimitSwitchStart()?  QStringLiteral("start"): QStringLiteral("-----"));
	list<< (c.hasLimitSwitchEnd()?    QStringLiteral("endli"): QStringLiteral("-----"));

	return list.join(", ");
}

QString actuatorConfigToString(const ArduMYActuatorConfig &c)
{
	QString ret;
	ret+=(
			 //QString(" flags=%1").arg(c.flags,8,2,QChar('0'))
			 actuatorConfigFlagsToString(c)
			 +QStringLiteral("; rep=")
			 +actuatorValueRepresentationToString(c.representation)
		 );
	return ret;
}


QString actuatorStateFlagsToString(const ArduMYActuatorState &s)
{
	QStringList list;
	list<< (s.isDirty()?              QStringLiteral("dirty"): QStringLiteral("-----"));
	list<< (s.isLimp()?               QStringLiteral("limpp"): QStringLiteral("-----"));
	return list.join(", ");
}


QString actuatorStateToString(const ArduMYActuatorState &s, const ArduMYActuatorValueRepresentation &rep)
{
	QString ret;
	ret+=(
			 //QString(" flags=%1").arg(s.flags,8,2,QChar('0'))
			 actuatorStateFlagsToString(s)
			 +QStringLiteral("; value=")+valueToString(s.value,rep)
		 );
	return ret;
}


QString actuatorToString(const ArduMYActuator &a)
{
	QString ret;
	ret+=QStringLiteral("config{")+actuatorConfigToString(a.config)+QStringLiteral("}, state{")+actuatorStateToString(a.state, a.config.representation)+QStringLiteral("}");
	return ret;
}


QString actuatorSetToString(const ArduMYActuatorSet &set)
{
	QString ret;
	ret+=(QString("ActuatorSet with size ")+QString::number(set.size())+QStringLiteral(":\n"));
	const auto size=set.size();
	for(size_t i=0; i<size; ++i) {
		const ArduMYActuator &a=set[i];
		ret+=QStringLiteral(" + ") + actuatorToString(a)+QStringLiteral("\n");
	}
	return ret;
}

void logLines(const QString ret)
{
	QStringList list=ret.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
	for(QString str:list) {
		qDebug().noquote().nospace()<<str;
	}
}

////////////////////////////////////////////////////////////////////////////////



static void randomValue(ArduMYActuatorValue &v, ArduMYActuatorValueRepresentation rep)
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



ArduMYActuatorConfig TestArduMY::randomConfig() const
{
	ArduMYActuatorConfig c;
	// Set random flags
	c.flags=0x00;
	c.setLinear(percentChance(50));
	c.setContinuous(percentChance(50));
	c.setGearRatio(percentChance(50));
	c.setPositionFeedback(percentChance(50));
	c.setTachometer(percentChance(50));
	c.setIncrementalEncoder(percentChance(50));
	c.setAbsoluteEncoder(percentChance(50));
	c.setLimitSwitchStart(percentChance(50));
	c.setLimitSwitchEnd(percentChance(50));
	c.setDirty(percentChance(50));

	// Set random type
	c.type=(ArduMYActuatorType)(qrand() % ((quint8)ArduMYActuatorType::TYPE_COUNT));

	// Set random representation

	c.representation=(ArduMYActuatorValueRepresentation)(qrand() % ((quint8)ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT));

	// All but BIT is ok
	//do {		c.representation=(ActuatorValueRepresentation)(qrand() % ((quint8)ActuatorValueRepresentation::REPRESENTATION_COUNT)) ;	} while (ActuatorValueRepresentation::BIT==c.representation);

	// All the same
	c.representation=ArduMYActuatorValueRepresentation::BYTE;

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
	case(ArduMYActuatorValueRepresentation::BIT): {
		c.rangeStart.byte=0;
		c.rangeSpan.byte=0;
		c.rangeStart.bit=false;
		c.rangeSpan.bit=true;
	}
	break;
	case(ArduMYActuatorValueRepresentation::BYTE): {
		c.rangeStart.byte=qrand()%qrand()%(0xFF);
		c.rangeSpan.byte=qrand()%((0xFF)-c.rangeStart.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::WORD): {
		c.rangeStart.word=qrand()%qrand()%(0xFFFF);
		c.rangeSpan.word=qrand()%((0xFFFF)-c.rangeStart.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
		c.rangeStart.doubleWord=qrand()%qrand()%(0xFFFFFFFF);
		c.rangeSpan.doubleWord=qrand()%((0xFFFFFFFF)-c.rangeStart.doubleWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
		// Naive attempt at making 64bit random numbers using qrand()
		uint64_t r1=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		uint64_t r2=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		uint64_t r3=((uint64_t)(qrand()%(0xFFFFFFFF)))<<32 | ((uint64_t)(qrand()%(0xFFFFFFFF)));
		c.rangeStart.quadWord=r1%r2%(0xFFFFFFFFFFFFFFFF);
		c.rangeSpan.quadWord=r3%((0xFFFFFFFFFFFFFFFF)-c.rangeStart.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
		float r1=frand();
		float r2=frand();
		float r3=frand();
		c.rangeStart.singlePrecision=r1*r2;
		c.rangeSpan.singlePrecision=r3-c.rangeStart.singlePrecision;
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
		double r1=dfrand();
		double r2=dfrand();
		double r3=dfrand();
		c.rangeStart.doublePrecision=r1*r2;
		c.rangeSpan.doublePrecision=r3-c.rangeStart.doublePrecision;
	}
	break;
	default:
	case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT): {
		qWarning()<<"bad representation";
	}
	break;
	}

	// Return resulting random config
	return c;
}


ArduMYActuatorState TestArduMY::randomState(const ArduMYActuatorConfig &c) const
{
	ArduMYActuatorState s;
	s.flags=0x00;
	s.setDirty(percentChance(50));
	s.setLimp(percentChance(50));
	randomValue(s.value, c.representation);
	return s;
}


ArduMYActuator TestArduMY::randomActuator() const
{
	ArduMYActuator a;
	a.config=randomConfig();
	a.state=randomState(a.config);
	return a;
}



ArduMYActuatorSet TestArduMY::randomActuatorSet()
{
	ArduMYActuatorSet set;
	//qsrand(2);
	const auto setSize=qrand()%10;
	set.setSize(setSize);
	if((uint32_t)set.size() != (uint32_t)setSize) {
		qWarning()<<"ERROR: set size was incorrect";
	}

	for(auto i=0; i<setSize; ++i) {
		ArduMYActuator a=randomActuator();
		ArduMYActuator &b=set[i];
		b=a;
	}
	return set;
}



ArduMYActuatorSet TestArduMY::fixedActuatorSet()
{
	ArduMYActuatorSet set=randomActuatorSet();
	const auto setSize=3;
	set.setSize(setSize);

	ArduMYActuator &b=set[0];
	{
		//Actuator a=randomActuator();
		//a.config.representation=ActuatorValueRepresentation::BYTE;		a.state.value.quadWord=0;		a.state.value.byte=133;		a.state.setDirty(true);
		b.config.representation=ArduMYActuatorValueRepresentation::BYTE;
		b.state.value.quadWord=0;
		b.state.value.byte=133;
		b.state.setDirty(true);
		//b=a;
	}

	b=set[1];
	{
		//Actuator a=randomActuator();
		//a.config.representation=ActuatorValueRepresentation::BIT;		a.state.value.quadWord=0;		a.state.value.bit=true;		a.state.setDirty(false);
		b.config.representation=ArduMYActuatorValueRepresentation::BIT;
		b.state.value.quadWord=0;
		b.state.value.bit=true;
		b.state.setDirty(false);
		//b=a;
	}

	b=set[2];
	{
		//Actuator a=randomActuator();
		//a.config.representation=ActuatorValueRepresentation::DOUBLE_FLOAT;		a.state.value.quadWord=0;		a.state.value.doublePrecision=3.14159265;		a.state.setDirty(true);
		b.config.representation=ArduMYActuatorValueRepresentation::DOUBLE_FLOAT;
		b.state.value.quadWord=0;
		b.state.value.doublePrecision=3.14159265;
		b.state.setDirty(true);
		//b=a;
	}

	b=set[1];

	//QCOMPARE(ActuatorValueRepresentation::BIT, b.config.representation);	QCOMPARE(true, b.state.value.bit);	QCOMPARE(false, b.state.isDirty());

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

ArduMYActuatorConfig TestArduMY::fuzzConfig() const
{
	Fuzzer<ArduMYActuatorConfig> fuzzer;
	// Return resulting fuzzed config
	return fuzzer.object;
}



ArduMYActuatorState TestArduMY::fuzzState() const
{
	Fuzzer<ArduMYActuatorState> fuzzer;
	// Return resulting fuzzed state
	return fuzzer.object;
}




ArduMYActuator TestArduMY::fuzzActuator() const
{
	Fuzzer<ArduMYActuator> fuzzer;
	// Return resulting fuzzed actuator
	return fuzzer.object;
}



ArduMYActuatorSet TestArduMY::fuzzActuatorSet()
{
	ArduMYActuatorSet set;
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

void TestArduMY::testDynamicArrayFundamental()
{
	DO_LOOPS_START("DynamicArrayFundamental", LOOPS)
	DynamicArray<double> array;
	QCOMPARE(array.size(), (uint32_t)0);
	for(uint32_t i =0 ; i<100; ++i) {
		const uint32_t oldSize=array.size();
		const uint32_t size=qrand()%20;
		const uint32_t compareSize=((size<oldSize)?size:oldSize);
		//qDebug().nospace().noquote()<<"["<<i<<"]- OLDSIZE: "<<oldSize<<", SIZE: "<<size<<", compareSize: "<<compareSize;
		for(uint32_t j=0 ; j< oldSize; ++j) {
			double &t=array[j];
			t=dfrand();
			//qDebug().nospace().noquote()<<"  +  "<<j<<": "<<t;
		}
		DynamicArray<double> arrayCopy=array;
		array.setSize(size);
		QCOMPARE(array.size(), size);
		QCOMPARE(arrayCopy.size(), oldSize);
		for(uint32_t j=0 ; j< compareSize; ++j) {
			{
				const double &a=array[j];
				const double &b=arrayCopy[j];
				//qDebug().nospace().noquote()<<"   A="<<a<<" vs. B="<<b<<" const "<<j;
				QCOMPARE(a,b);

			}
			{
				double &a=array[j];
				double &b=arrayCopy[j];
				//qDebug().nospace().noquote()<<"   A="<<a<<" vs. B="<<b<<" "<<j;
				QCOMPARE(a,b);
			}
		}
		for(uint32_t j=compareSize ; j< size; ++j) {
			{
				const double &a=array[j];
				QCOMPARE(a, (double)0);
			}
			{
				double &a=array[j];
				QCOMPARE(a, (double)0);
			}
		}
	}
	DO_LOOPS_END
}

void TestArduMY::testDynamicArrayPOD()
{
	DO_LOOPS_START("DynamicArrayPOD", LOOPS)
	struct Test {
		uint32_t a;
		double b;
		bool c;

		Test()
			: a() // Value-initialize (set to 0 or equivalent)
			, b()
			, c()
		{
			//qDebug()<<"TEST CTOR CALLED";
		}
	};

	DynamicArray<Test> array;
	QCOMPARE(array.size(), (uint32_t)0);
	for(uint32_t i =0 ; i<100; ++i) {
		const uint32_t oldSize=array.size();
		const uint32_t size=qrand()%20;
		const uint32_t compareSize=((size<oldSize)?size:oldSize);
		//qDebug().nospace().noquote()<<"["<<i<<"]- OLDSIZE: "<<oldSize<<", SIZE: "<<size<<", compareSize: "<<compareSize;
		for(uint32_t j=0 ; j< oldSize; ++j) {
			Test &t=array[j];
			t.a=qrand();
			t.b=dfrand();
			t.c=percentChance(50);
			//qDebug().nospace().noquote()<<"  +  "<<j<<": A="<<t.a<<" B="<<t.b<<" C="<<t.c;
		}
		DynamicArray<Test> arrayCopy=array;
		array.setSize(size);
		QCOMPARE(array.size(), size);
		QCOMPARE(arrayCopy.size(), oldSize);
		for(uint32_t j=0 ; j< compareSize; ++j) {
			{
				const Test &a=array[j];
				const Test &b=arrayCopy[j];
				//qDebug().nospace().noquote()<<"   A={"<<a.a<<", "<<a.b<<", "<<a.c<<"} vs. B={"<<b.a<<", "<<b.b<<", "<<b.c<<"} const "<<j;
				QCOMPARE(a.a,b.a);
				QCOMPARE(a.b,b.b);
				QCOMPARE(a.c,b.c);
			}
			{
				Test &a=array[j];
				Test &b=arrayCopy[j];
				//qDebug().nospace().noquote()<<"   A={"<<a.a<<", "<<a.b<<", "<<a.c<<"} vs. B={"<<b.a<<", "<<b.b<<", "<<b.c<<"} "<<j;
				QCOMPARE(a.a,b.a);
				QCOMPARE(a.b,b.b);
				QCOMPARE(a.c,b.c);
			}
		}
		for(uint32_t j=compareSize ; j< size; ++j) {
			{
				const Test &a=array[j];
				QCOMPARE(a.a, (uint32_t)0);
				QCOMPARE(a.b, (double)0);
				QCOMPARE(a.c, false);
			}
			{
				Test &a=array[j];
				QCOMPARE(a.a, (uint32_t)0);
				QCOMPARE(a.b, (double)0);
				QCOMPARE(a.c, false);
			}

		}

	}
	DO_LOOPS_END
}

void TestArduMY::testActuatorValue()
{
	DO_LOOPS_START("ActuatorValue", LOOPS)
	for(int i=0; i<(int)ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT; ++i) {
		ArduMYActuatorValueRepresentation rep=(ArduMYActuatorValueRepresentation)i;
		ArduMYActuatorValue a;
		QCOMPARE(a.bit,false);
		QCOMPARE(a.byte,(typeof(a.byte))0x00);
		QCOMPARE(a.word,(typeof(a.word))0x00);
		QCOMPARE(a.doubleWord,(typeof(a.doubleWord))0x00);
		QCOMPARE(a.singlePrecision,(typeof(a.singlePrecision))0.0f);
		QCOMPARE(a.doublePrecision,(typeof(a.doublePrecision))0.0f);
		randomValue(a,rep);
		ArduMYActuatorValue b;
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
	for(int i=0; i<(int)ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT; ++i) {
		ArduMYActuatorValueRepresentation rep=(ArduMYActuatorValueRepresentation)i;
		Converter a;
		switch(rep) {
		default:
		case(BIT): {
			for(int j=0; j<8; ++j) {
				QCOMPARE(a.int8[j], (int8_t)0);
			}
			for(int j=0; j<8; ++j) {
				QCOMPARE(a.uint8[j], (uint8_t)0);
			}
		}
		break;
		case(BYTE): {
			for(int j=0; j<8; ++j) {
				QCOMPARE(a.int8[j], (int8_t)0);
			}
			for(int j=0; j<8; ++j) {
				QCOMPARE(a.uint8[j], (uint8_t)0);
			}
		}
		break;
		case(WORD): {
			for(int j=0; j<4; ++j) {
				QCOMPARE(a.int16[j], (int16_t)0);
			}
			for(int j=0; j<4; ++j) {
				QCOMPARE(a.uint16[j], (uint16_t)0);
			}
		}
		break;
		case(DOUBLE_WORD): {
			for(int j=0; j<2; ++j) {
				QCOMPARE(a.int32[j], (int32_t)0);
			}
			for(int j=0; j<2; ++j) {
				QCOMPARE(a.uint32[j], (uint32_t)0);
			}
		}
		break;
		case(QUAD_WORD): {
			QCOMPARE(a.int64, (typeof(a.int64))0);
			QCOMPARE(a.uint64, (typeof(a.uint64))0);
		}
		break;
		case(SINGLE_FLOAT): {
			for(int j=0; j<2; ++j) {
				QCOMPARE(a.float32[j], (float)0.0f);
			}
		}
		break;
		case(DOUBLE_FLOAT): {
			QCOMPARE(a.float64, (double)0.0);
			break;
		}
		}
		ArduMYActuatorValue v;
		randomValue(v, rep);
		switch(rep) {
		default:
		case(BIT):
		case(BYTE): {
			a.int8[0]=v.byte;
		}
		break;
		case(WORD): {
			a.int16[0]=v.word;
		}
		break;
		case(DOUBLE_WORD): {
			a.int32[0]=v.doubleWord;
		}
		break;
		break;
		case(QUAD_WORD): {
			a.int64=v.quadWord;
		}
		break;
		case(SINGLE_FLOAT): {
			a.float32[0]=v.singlePrecision;
		}
		break;
		case(DOUBLE_FLOAT): {
			a.float64=v.doublePrecision;
		}
		break;
		}
		Converter b;
		a=b;
		for(int j=0; j<8; ++j) {
			QCOMPARE(a.int8[j], b.int8[j]);
		}
		for(int j=0; j<4; ++j) {
			QCOMPARE(a.int16[j], b.int16[j]);
		}
		for(int j=0; j<2; ++j) {
			QCOMPARE(a.int32[j], b.int32[j]);
		}
		for(int j=0; j<2; ++j) {
			QCOMPARE(a.float32[j], b.float32[j]);
		}
		for(int j=0; j<8; ++j) {
			QCOMPARE(a.uint8[j], b.uint8[j]);
		}
		for(int j=0; j<4; ++j) {
			QCOMPARE(a.uint16[j], b.uint16[j]);
		}
		for(int j=0; j<2; ++j) {
			QCOMPARE(a.uint32[j], b.uint32[j]);
		}
		QCOMPARE(a.int64, b.int64);
		QCOMPARE(a.float64, b.float64);
		//QCOMPARE(a,b);
	}
	DO_LOOPS_END
}



void TestArduMY::testActuatorRandomConfigParser()
{
	DO_LOOPS_START("ActuatorConfigEquals Random", LOOPS)
	const ArduMYActuatorConfig a=randomConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END

}

void TestArduMY::testActuatorRandomConfigSerializer()
{
	DO_LOOPS_START("ActuatorConfigSerializer Random", LOOPS)
	ArduMYActuatorConfig c=randomConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigSerializer.inc.hpp"
	DO_LOOPS_END
}


void TestArduMY::testActuatorRandomConfigEquals()
{
	DO_LOOPS_START("ActuatorConfigEquals Random", LOOPS)
	const ArduMYActuatorConfig a=randomConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END
}




void TestArduMY::testActuatorRandomValueParser()
{
	DO_LOOPS_START("ActuatorValueParser Random", LOOPS)
	ArduMYActuatorSet inSet=randomActuatorSet();
	if(inSet.size()>0) {
#		define TEST_INCLUDED
#		include "testActuatorValueParser.inc.hpp"
	}
	DO_LOOPS_END
}




void TestArduMY::testActuatorRandomValueEquals()
{
	DO_LOOPS_START("ActuatorValueEquals Random", LOOPS)
	ArduMYActuatorSet a=randomActuatorSet();
#		define TEST_INCLUDED
#		include "testActuatorValueEquals.inc.hpp"
	DO_LOOPS_END
}



void TestArduMY::testActuatorRandomValueSerializer()
{
	DO_LOOPS_START("ActuatorValueSerializer Random", LOOPS)
	ArduMYActuatorSet inSet=randomActuatorSet();
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
	const ArduMYActuatorConfig a=fuzzConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END
}


void TestArduMY::testActuatorFuzzConfigParser()
{
	DO_LOOPS_START("ActuatorConfigParser Fuzz", LOOPS)
	ArduMYActuatorConfig c=fuzzConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigParser.inc.hpp"
	DO_LOOPS_END
}


void TestArduMY::testActuatorFuzzConfigEquals()
{
	DO_LOOPS_START("ActuatorConfigEquals Fuzz", LOOPS)
	const ArduMYActuatorConfig a=fuzzConfig();
#		define TEST_INCLUDED
#		include "testActuatorConfigEquals.inc.hpp"

	DO_LOOPS_END
}

void TestArduMY::testActuatorFuzzValueParser()
{
	DO_LOOPS_START("ActuatorValueParser Fuzz", LOOPS)
	ArduMYActuatorSet inSet=fuzzActuatorSet();
	if(inSet.size()>0) {
#		define TEST_INCLUDED
#		include "testActuatorValueParser.inc.hpp"
	}
	DO_LOOPS_END
}


void TestArduMY::testActuatorFuzzValueSerializer()
{
	DO_LOOPS_START("ActuatorValueSerializer Fuzz", LOOPS)
	ArduMYActuatorSet inSet=fuzzActuatorSet();
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
	ArduMYActuatorSet a=fuzzActuatorSet();
#		define TEST_INCLUDED
#		include "testActuatorValueEquals.inc.hpp"
	DO_LOOPS_END
}


void TestArduMY::testRepresentationBoundary()
{
	ArduMYActuatorSet inSet;
	ArduMYActuatorSet outSet;
	inSet.setSize(4);
	outSet.setSize(4);
	inSet[0].config.representation=ArduMYActuatorValueRepresentation::BYTE;
	inSet[0].state.value.quadWord=0;
	inSet[0].state.value.byte=111;
	inSet[1].config.representation=ArduMYActuatorValueRepresentation::BYTE;
	inSet[1].state.value.quadWord=0;
	inSet[1].state.value.byte=222;
	inSet[2].config.representation=ArduMYActuatorValueRepresentation::WORD;
	inSet[2].state.value.quadWord=0;
	inSet[2].state.value.word=33333;
	inSet[3].config.representation=ArduMYActuatorValueRepresentation::WORD;
	inSet[3].state.value.quadWord=0;
	inSet[3].state.value.word=44444;
	for(size_t i =0; i<4; ++i) {
		outSet[i].config.representation=inSet[i].config.representation;
		outSet[i].state.value.quadWord=0;
	}
	ArduMYActuatorValueParser parser;

	QCOMPARE(parser.set, (ArduMYActuatorSet *)nullptr);
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
	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ArduMYActuatorValueRepresentation::BIT);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)0);

	//All enabled
	const uint8_t enabled=1|2|4|8;
	parser.parse(enabled);

	for(uint8_t v=0; v<2; ++v) {

		QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ArduMYActuatorValueRepresentation::BYTE);
		QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)v);
		QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
		parser.parse(inSet[v].state.value.byte);
		QCOMPARE(parser.byteIndex, (uint16_t) 0);
		QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
		QCOMPARE(parser.enableBits[0], enabled);
		for(int i=1; i<32; ++i) {
			QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
		}

	}
	for(uint8_t v=0; v<2; ++v) {
		for(uint8_t b=0; b<2; ++b) {
			QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ArduMYActuatorValueRepresentation::WORD);
			QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)(2+v));
			QCOMPARE((uint8_t)parser.step, (uint8_t)ActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
			Converter cv;
			cv.uint64=0;
			cv.uint16[0]=inSet[2+v].state.value.word;
			parser.parse(cv.uint8[b]);
			QCOMPARE(parser.byteIndex, (uint16_t)((b+1)%2));
			QCOMPARE(parser.enabledActuatorCount, (uint8_t)4);
			QCOMPARE(parser.enableBits[0], enabled);
			for(int i=1; i<32; ++i) {
				QCOMPARE(parser.enableBits[i], (uint8_t)0x00);
			}
		}
	}

	QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT);
	QCOMPARE((int16_t)parser.currentActuatorIndex, (int16_t)0);

}


void TestArduMY::testCommandParser()
{
	CommandParser parser;
	return;

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
		ArduMYActuatorSet set=randomActuatorSet();

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

			ArduMYActuator &a=set[actuatorIndex];
			ArduMYActuatorConfig &c=a.config;

			ArduMYActuatorConfigSerializer serializer;

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
				ArduMYActuator &a=set[actuatorIndex];
				const bool dirty=percentChance(i*11);
				a.state.setDirty(dirty);
				randomValue(a.state.value, a.config.representation);
				qDebug()<<" ACTUATOR "<<actuatorIndex<<"/"<<set.size()<<" was set to "<<(dirty?"DIRTY":"CLEAN")<<" with value "<<valueToString(a.state.value,a.config.representation);
			}
			ArduMYActuatorValueSerializer serializer;
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
