include($$TESTS/test.pri)

TARGET = test_creepyvoice

HEADERS += \
	TestCreepyVoice.hpp

SOURCES += \
	TestCreepyVoice.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

