include($$TESTS/pris/test.pri)

TARGET = test_parser

HEADERS += \
	TestParser.hpp

SOURCES += \
	TestParser.cpp

include($$TESTS/pris/test_autorun.pri)
