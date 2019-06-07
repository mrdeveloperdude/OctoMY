include($$TESTS/pris/test.pri)

TARGET = test_fivephase

HEADERS += \
	TestFivePhase.hpp

SOURCES += \
	TestFivePhase.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

