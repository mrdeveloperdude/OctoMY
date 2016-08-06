include($$TOP_PWD/test/test.pri)

TARGET = test_portableid

HEADERS += \
	TestPortableID.hpp

SOURCES += \
	TestPortableID.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

