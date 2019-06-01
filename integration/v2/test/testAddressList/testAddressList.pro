include($$TESTS/pris/test.pri)

TARGET = test_addresslist

HEADERS += \
	TestAddressList.hpp

SOURCES += \
	TestAddressList.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
