include($$TESTS/pris/test.pri)

TARGET = test_networkaddress

HEADERS += \
	TestNetworkAddress.hpp

SOURCES += \
	TestNetworkAddress.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
