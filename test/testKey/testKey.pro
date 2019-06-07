include($$TESTS/pris/test.pri)

TARGET = test_key

HEADERS += \
	TestKey.hpp

SOURCES += \
	TestKey.cpp

include($$TESTS/pris/test_autorun.pri)
