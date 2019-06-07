include($$TESTS/pris/test.pri)

TARGET = test_ratecalculator

HEADERS += \
	TestRateCalculator.hpp

SOURCES += \
	TestRateCalculator.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
