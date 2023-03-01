#include "TestNameGenerator.hpp"

#include "name/RemoteNameGenerator.hpp"
#include "name/AgentNameGenerator.hpp"


void TestNameGenerator::testAgent()
{
	AgentNameGenerator ang;
	for(int i=0; i<100; ++i) {
		QString out = ang.generate();
		qDebug()<<"AGENT: "<<out;
	}
}


void TestNameGenerator::testRemote()
{
	RemoteNameGenerator rng;
	for(int i=0; i<100; ++i) {
		QString out = rng.generate();
		qDebug()<<"REMOTE: "<<out;
	}
}


OC_TEST_MAIN(test, TestNameGenerator)
