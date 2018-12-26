include($$TESTS/pris/test.pri)

TARGET = test_template

HEADERS += \
	TestTemplate.hpp

SOURCES += \
	TestTemplate.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

