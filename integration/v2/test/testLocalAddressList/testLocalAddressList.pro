include($$TESTS/pris/test.pri)

TARGET = test_localaddresslist

HEADERS += \
	TestLocalAddressList.hpp

SOURCES += \
	TestLocalAddressList.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

