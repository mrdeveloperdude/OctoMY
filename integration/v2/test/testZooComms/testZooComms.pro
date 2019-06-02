include($$TESTS/pris/test.pri)

TARGET = test_zoocomms

HEADERS += \
	TestZooComms.hpp

SOURCES += \
	TestZooComms.cpp

include($$TESTS/test_autorun.pri)
