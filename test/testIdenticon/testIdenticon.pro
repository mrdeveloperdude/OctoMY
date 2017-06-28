include($$TESTS/test.pri)

TARGET = test_identicon

HEADERS += \
	TestIdenticon.hpp

SOURCES += \
	TestIdenticon.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
