#ifndef TESTAUDIOSTREAM_HPP
#define TESTAUDIOSTREAM_HPP

#include "test/Common.hpp"

class TestAudioStream:public QObject
{
	Q_OBJECT
private slots:
	void testAudioInputPreview();
	void testAudioSourceSelector();

};




#endif
// TESTAUDIOSTREAM_HPP
