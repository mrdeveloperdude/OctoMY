include($$TESTS/test.pri)

TARGET = test_ratecalculator

HEADERS += \
	TestRateCalculator.hpp

SOURCES += \
	TestRateCalculator.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

