include($$TESTS/test.pri)

TARGET = test_portableid

HEADERS += \
	TestPortableID.hpp

SOURCES += \
	TestPortableID.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

