include($$TESTS/test.pri)

TARGET = test_hashstore

HEADERS += \
	TestHashstore.hpp

SOURCES += \
	TestHashstore.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
