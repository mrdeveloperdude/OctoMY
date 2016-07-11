include($$TOP_PWD/test/test.pri)

TARGET = test_zoocomms

HEADERS += \
	TestZooComms.hpp

SOURCES += \
	TestZooComms.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
