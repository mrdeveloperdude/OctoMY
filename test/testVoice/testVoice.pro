include($$TESTS/pris/test.pri)

TARGET = test_voice

HEADERS += \
	TestVoice.hpp

SOURCES += \
	TestVoice.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

