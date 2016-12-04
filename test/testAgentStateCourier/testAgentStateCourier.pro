include($$TOP_PWD/test/test.pri)

TARGET = test_agentstatecourier

HEADERS += \
	TestAgentStateCourier.hpp

SOURCES += \
	TestAgentStateCourier.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

