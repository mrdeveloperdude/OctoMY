#include "TestCreepyVoice.hpp"

#include "audio/sources/CreepyVoice.hpp"
#include "security/PortableID.hpp"


// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestCreepyVoice::test()
{
	PortableID id;
	
	CreepyVoice<double, 2> cv;
	cv.speak(id, "LOL");
	quint32 num=100;
	double buf[num];
	cv.generate(num, buf);
}


OC_TEST_MAIN(test, TestCreepyVoice)
