include($$TESTS/test.pri)

TARGET = test_rng

HEADERS += \
	TestRNG.hpp

SOURCES += \
	TestRNG.cpp

include($$TESTS/test_autorun.pri)
