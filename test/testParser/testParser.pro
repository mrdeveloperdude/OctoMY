include($$TESTS/test.pri)

TARGET = test_parser

HEADERS += \
	TestParser.hpp

SOURCES += \
	TestParser.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
