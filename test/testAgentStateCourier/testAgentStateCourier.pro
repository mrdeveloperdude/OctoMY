include($$TESTS/pris/test.pri)

TARGET = test_agentstatecourier

HEADERS += \
	TestAgentStateCourier.hpp

SOURCES += \
	TestAgentStateCourier.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

