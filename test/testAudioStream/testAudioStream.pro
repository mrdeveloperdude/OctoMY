include($$TESTS/pris/test.pri)

TARGET = test_audiostream

HEADERS += \
	TestAudioStream.hpp

SOURCES += \
	TestAudioStream.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

