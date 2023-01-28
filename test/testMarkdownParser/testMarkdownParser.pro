include($$TESTS/pris/test.pri)

TARGET = test_markdownparser

HEADERS += \
	TestMarkdownParser.hpp

SOURCES += \
	TestMarkdownParser.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

