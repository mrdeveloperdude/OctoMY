include($$TESTS/pris/test.pri)

TARGET = test_namegenerator

HEADERS += \
	TestNameGenerator.hpp

SOURCES += \
	TestNameGenerator.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
