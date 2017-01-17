#ifndef TEST_INCLUDED


#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"


#include "../arduino/ActuatorValueParser.hpp"
#include "../arduino/ActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QTest>

void testActuatorValueSerializerInclude(ActuatorSet &inSet)
{
#endif


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

	uint32_t loopCounter=0;
	while(serializer.hasMoreData()) {
		uint8_t byte=serializer.nextByte();
		parser.parse(byte);
		if(loopCounter++ > 1000000) {
			qWarning()<<"ERROR: LOOP NEVER ENDED";
			break;
		}
	}

	for(size_t i =0; i<inSize; ++i) {
		outSet[i].isEqual(inSet[i]);
	}


	QCOMPARE(inSet,outSet);


#ifndef TEST_INCLUDED
}
#endif
