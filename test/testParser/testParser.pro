include($$TOP_PWD/test/test.pri)

TARGET = test_parser

HEADERS += \
	TestParser.hpp

SOURCES += \
	TestParser.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
