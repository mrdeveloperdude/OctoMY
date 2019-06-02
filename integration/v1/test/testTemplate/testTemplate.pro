include($$TESTS/test.pri)

TARGET = test_template

HEADERS += \
	TestTemplate.hpp

SOURCES += \
	TestTemplate.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

