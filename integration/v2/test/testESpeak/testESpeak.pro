include($$TESTS/pris/test.pri)

TARGET = test_espeak

HEADERS += \
	TestESpeak.hpp

SOURCES += \
	TestESpeak.cpp

include($$TESTS/test_autorun.pri)
