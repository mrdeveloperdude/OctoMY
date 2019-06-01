include($$TESTS/pris/test.pri)

TARGET = test_identicon

HEADERS += \
	TestIdenticon.hpp

SOURCES += \
	TestIdenticon.cpp

include($$TESTS/test_autorun.pri)
