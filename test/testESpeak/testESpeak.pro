include($$TESTS/pris/test.pri)

TARGET = test_espeak

HEADERS += \
	TestESpeak.hpp

SOURCES += \
	TestESpeak.cpp

include($$TESTS/pris/test_autorun.pri)
