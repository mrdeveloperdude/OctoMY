include($$TOP_PWD/test/test.pri)

TARGET = test_creepyvoice

HEADERS += \
	TestCreepyVoice.hpp

SOURCES += \
	TestCreepyVoice.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

