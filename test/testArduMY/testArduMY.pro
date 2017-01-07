include($$TOP_PWD/test/test.pri)

TARGET = test_ardumy

HEADERS += \
	TestArduMY.hpp \
	../../libs/arduino/MagicDetector.hpp \
	../../libs/arduino/CommandParser.hpp \
	../../libs/arduino/SetServoPositionsParser.hpp \
	../../libs/arduino/ServoConfigParser.hpp \
	../../libs/arduino/Actuator.hpp \


SOURCES += \
	TestArduMY.cpp \
	../../libs/arduino/MagicDetector.cpp \
	../../libs/arduino/CommandParser.cpp \
	../../libs/arduino/SetServoPositionsParser.cpp \
	../../libs/arduino/ServoConfigParser.cpp \
	../../libs/arduino/Actuator.cpp \

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

