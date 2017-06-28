include($$TESTS/test.pri)

TARGET = test_zoocomms

HEADERS += \
	TestZooComms.hpp

SOURCES += \
	TestZooComms.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
