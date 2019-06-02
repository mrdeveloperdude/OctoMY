include($$TESTS/test.pri)

TARGET = test_fivephase

HEADERS += \
	TestFivePhase.hpp

SOURCES += \
	TestFivePhase.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

