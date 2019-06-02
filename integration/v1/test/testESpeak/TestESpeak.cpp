#include "TestESpeak.hpp"

#include "audio/CreepyVoice.hpp"
#include "audio/AudioStream.hpp"
#include "security/PortableID.hpp"

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestESpeak::test()
{

	PortableID id;
	CreepyVoice voice(id);

	AudioStream as(voice);

	as.init();

	QTest::qWait(1000);

	voice.speak("Octomy will surely take over the world of robotics.");
	QTest::qWait(1000);
	voice.speak("There really isn't any doubt about that.");
	QTest::qWait(10000);

}



OC_TEST_MAIN(test, TestESpeak)
