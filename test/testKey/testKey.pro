include($$TESTS/pris/test.pri)

TARGET = test_key

HEADERS += \
	TestKey.hpp

SOURCES += \
	TestKey.cpp

include($$TESTS/test_autorun.pri)
