include($$TOP_PWD/test/test.pri)

TARGET = test_espeak

HEADERS += \
	TestESpeak.hpp

SOURCES += \
	TestESpeak.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

