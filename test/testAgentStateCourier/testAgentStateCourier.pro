include($$TESTS/test.pri)

TARGET = test_agentstatecourier

HEADERS += \
	TestAgentStateCourier.hpp

SOURCES += \
	TestAgentStateCourier.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

