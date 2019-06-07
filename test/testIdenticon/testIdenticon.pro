include($$TESTS/pris/test.pri)

TARGET = test_identicon

HEADERS += \
	TestIdenticon.hpp

SOURCES += \
	TestIdenticon.cpp

include($$TESTS/pris/test_autorun.pri)
