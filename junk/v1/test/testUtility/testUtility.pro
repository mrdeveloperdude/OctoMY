include($$TESTS/test.pri)

TARGET = test_utility

HEADERS += \
	TestUtility.hpp

SOURCES += \
	TestUtility.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

