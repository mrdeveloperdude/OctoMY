include($$TOP_PWD/test/test.pri)

TARGET = test_hashstore

HEADERS += \
	TestHashstore.hpp

SOURCES += \
	TestHashstore.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
