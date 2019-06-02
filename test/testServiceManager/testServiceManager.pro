include($$TESTS/pris/test.pri)

TARGET = test_servicemanager

HEADERS += \
	TestServiceManager.hpp

SOURCES += \
	TestServiceManager.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

