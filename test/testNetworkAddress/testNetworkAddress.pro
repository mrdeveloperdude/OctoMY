include($$TESTS/test.pri)

TARGET = test_networkaddress

HEADERS += \
	TestNetworkAddress.hpp

SOURCES += \
	TestNetworkAddress.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

