#include "TestVoice.hpp"

#include "widgets/voice/SpeechControlWidget.hpp"

#include "Utility_test.hpp"

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

