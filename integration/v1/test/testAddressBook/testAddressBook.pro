include($$TESTS/test.pri)

TARGET = test_addressbook

HEADERS += \
	TestAddressBook.hpp

SOURCES += \
	TestAddressBook.cpp

include($$TESTS/test_autorun.pri)

