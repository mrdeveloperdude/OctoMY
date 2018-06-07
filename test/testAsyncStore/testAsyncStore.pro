include($$TESTS/test.pri)

TARGET = test_asyncstore

HEADERS += \
	TestAsyncStore.hpp

SOURCES += \
	TestAsyncStore.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

