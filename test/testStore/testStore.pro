include($$TESTS/pris/test.pri)

TARGET = test_store

HEADERS += \
	TestStore.hpp

SOURCES += \
	TestStore.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

