include($$TESTS/test.pri)

TARGET = test_servotor32controller

HEADERS += \
	TestServotor32Controller.hpp

SOURCES += \
	TestServotor32Controller.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

