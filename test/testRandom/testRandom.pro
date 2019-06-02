include($$TESTS/pris/test.pri)

TARGET = test_random

HEADERS += \
	TestRandom.hpp

SOURCES += \
	TestRandom.cpp

include($$TESTS/test_autorun.pri)
