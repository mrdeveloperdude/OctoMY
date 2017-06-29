include($$TESTS/test.pri)

TARGET = test_hashstore

HEADERS += \
	TestHashstore.hpp

SOURCES += \
	TestHashstore.cpp

include($$TESTS/test_autorun.pri)
