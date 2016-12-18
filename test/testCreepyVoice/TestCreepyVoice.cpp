#include "TestCreepyVoice.hpp"

#include "audio/CreepyVoice.hpp"
#include "security/PortableID.hpp"


// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestCreepyVoice::test()
{
	PortableID id;
	CreepyVoice cv(id);
	cv.speak("LOL");
	quint32 num=100;
	double buf[num];
	cv.generate(num,buf);

}



QTEST_MAIN(TestCreepyVoice)
