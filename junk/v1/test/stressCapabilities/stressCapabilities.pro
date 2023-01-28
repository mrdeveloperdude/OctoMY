include($$TESTS/stress.pri)

TARGET = stress_capabilities

HEADERS += \
	StressCapabilities.hpp

SOURCES += \
	StressCapabilities.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

