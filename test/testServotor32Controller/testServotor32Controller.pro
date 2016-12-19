include($$TOP_PWD/test/test.pri)

TARGET = test_servotor32controller

HEADERS += \
	TestServotor32Controller.hpp

SOURCES += \
	TestServotor32Controller.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

