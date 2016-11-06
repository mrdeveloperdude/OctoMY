#include "TestESpeak.hpp"

#include "../libcore/audio/CreepyVoice.hpp"
#include "../libcore/audio/AudioStream.hpp"
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



QTEST_MAIN(TestESpeak)
