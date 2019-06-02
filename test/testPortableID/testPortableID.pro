include($$TESTS/pris/test.pri)

TARGET = test_portableid

HEADERS += \
	TestPortableID.hpp

SOURCES += \
	TestPortableID.cpp

include($$TESTS/test_autorun.pri)
