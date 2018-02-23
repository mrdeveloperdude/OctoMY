include($$TESTS/test.pri)

TARGET = test_datastore

HEADERS += \
	TestDataStore.hpp

SOURCES += \
	TestDataStore.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

