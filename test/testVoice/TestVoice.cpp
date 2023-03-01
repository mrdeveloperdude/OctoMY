#include "TestVoice.hpp"

#include "voice/SpeechControlWidget.hpp"

#include "test/Utility.hpp"

#include <QTimer>

void TestVoice::test()
{

	SpeechControlWidget w, *wp=&w;
	w.show();
	QTimer t;
	t.start(10);
	connect(&t, &QTimer::timeout, this, [=]() {
		
	});
	
	test::utility::waitForUIEnd(wp);
}


OC_TEST_MAIN(test, TestVoice)

