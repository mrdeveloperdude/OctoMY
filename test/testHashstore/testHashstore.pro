include($$TESTS/pris/test.pri)

TARGET = test_hashstore

HEADERS += \
	TestHashstore.hpp

SOURCES += \
	TestHashstore.cpp

include($$TESTS/pris/test_autorun.pri)
