include($$TESTS/pris/test.pri)

TARGET = test_discovery

HEADERS += \
	TestDiscovery.hpp

SOURCES += \
	TestDiscovery.cpp

include($$PRIS/status.pri)
