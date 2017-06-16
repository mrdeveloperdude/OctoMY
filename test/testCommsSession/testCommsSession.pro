include($$TESTS/test.pri)

TARGET = test_commssession

HEADERS += \
	TestCommsSession.hpp

SOURCES += \
	TestCommsSession.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

