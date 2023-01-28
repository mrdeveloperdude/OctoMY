#ifndef TESTAUDIOSTREAM_HPP
#define TESTAUDIOSTREAM_HPP

#include "Common_test.hpp"

class TestAudioStream:public QObject
{
	Q_OBJECT
private slots:
	void testAudioInputPreview();
	void testAudioSourceSelector();

};




#endif
// TESTAUDIOSTREAM_HPP
