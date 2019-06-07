include($$TESTS/pris/test.pri)

TARGET = test_addressbook

HEADERS += \
	TestAddressBook.hpp

SOURCES += \
	TestAddressBook.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
