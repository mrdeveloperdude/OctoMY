include($$TESTS/test.pri)

TARGET = test_key

HEADERS += \
	TestKey.hpp

SOURCES += \
	TestKey.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
