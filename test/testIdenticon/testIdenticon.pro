include($$TOP_PWD/test/test.pri)

TARGET = test_identicon

HEADERS += \
	TestIdenticon.hpp

SOURCES += \
	TestIdenticon.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
