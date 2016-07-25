include($$TOP_PWD/test/test.pri)

TARGET = test_networkaddress

HEADERS += \
	TestNetworkAddress.hpp

SOURCES += \
	TestNetworkAddress.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

