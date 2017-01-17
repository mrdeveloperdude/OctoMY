#ifndef TEST_INCLUDED


#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"


#include "../arduino/ActuatorValueParser.hpp"
#include "../arduino/ActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QTest>

void testActuatorConfigSerializerInclude(ActuatorSet &inSet)
{
#endif


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


#ifndef TEST_INCLUDED
}
#endif
