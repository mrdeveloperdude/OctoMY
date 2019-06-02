include($$TESTS/pris/test.pri)

TARGET = test_servotor32controller

HEADERS += \
	TestServotor32Controller.hpp

SOURCES += \
	TestServotor32Controller.cpp

include($$TESTS/test_autorun.pri)

