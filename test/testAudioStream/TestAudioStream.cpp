#include "TestAudioStream.hpp"

#include "audio/AudioInputPreview.hpp"
#include "audio/AudioSourceSelector.hpp"
#include "uptime/ConnectionType.hpp"

#include "test/Utility.hpp"

#include <QTimer>


void TestAudioStream::testAudioInputPreview(){
	AudioInputPreview w, *wp=&w;
	QString defaultAudioSourceName= "QuickCam Pro 9000 Mono";
	qDebug()<<"Default input device was "<<defaultAudioSourceName;
	w.setAudioInput(defaultAudioSourceName);
	w.show();
	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=]() {
		
	}, OC_CONTYPE_NON_UNIQUE);
	
	test::utility::waitForUIEnd(wp);
}


void TestAudioStream::testAudioSourceSelector(){
	AudioSourceSelector w, *wp=&w;
	//w.updateSources(true, true);
	w.show();
	QTimer t;
	t.start(10);
	connect(&t, &QTimer::timeout, this, [=]() {
		
	}, OC_CONTYPE_NON_UNIQUE);

	test::utility::waitForUIEnd(wp);
	w.updateSources(true, false);
	w.show();
	test::utility::waitForUIEnd(wp);
	w.updateSources(false, true);
	w.show();
	test::utility::waitForUIEnd(wp);
	
}


OC_TEST_MAIN(test, TestAudioStream)

