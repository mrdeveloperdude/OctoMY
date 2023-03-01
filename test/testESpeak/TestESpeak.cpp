#include "TestESpeak.hpp"

#include "audio/sources/CreepyVoice.hpp"
#include "audio/AudioStream.hpp"
#include "security/PortableID.hpp"


void TestESpeak::test()
{
	PortableID id;
	CreepyVoice<double, 2> voice;
/*
	AudioStream as(voice);

	as.init();
*/
	QTest::qWait(1000);

	voice.speak(id, "Octomy will surely take over the world of robotics.");
	QTest::qWait(1000);
	voice.speak(id, "There really isn't any doubt about that.");
	QTest::qWait(10000);
}


OC_TEST_MAIN(test, TestESpeak)
