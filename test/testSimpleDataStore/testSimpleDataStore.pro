include($$TESTS/pris/test.pri)

TARGET = test_simpledatastore

HEADERS += \
	TestSimpleDataStore.hpp

SOURCES += \
	TestSimpleDataStore.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
