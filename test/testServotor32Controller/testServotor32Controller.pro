include($$TESTS/pris/test.pri)

TARGET = test_servotor32controller

HEADERS += \
	TestServotor32Controller.hpp

SOURCES += \
	TestServotor32Controller.cpp

include($$TESTS/pris/test_autorun.pri)

