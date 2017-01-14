include($$TOP_PWD/test/test.pri)

TARGET = test_ardumy

HEADERS += \
	TestArduMY.hpp \
	../../libs/arduino/MagicDetector.hpp \
	../../libs/arduino/CommandParser.hpp \
	../../libs/arduino/Actuator.hpp \
	../../libs/arduino/ActuatorConfigSerializerBase.hpp \
	../../libs/arduino/ActuatorConfigSerializer.hpp \
	../../libs/arduino/ActuatorConfigParser.hpp \
	../../libs/arduino/ActuatorValueSerializerBase.hpp \
	../../libs/arduino/ActuatorValueSerializer.hpp \
	../../libs/arduino/ActuatorValueParser.hpp \


SOURCES += \
	TestArduMY.cpp \
	../../libs/arduino/MagicDetector.cpp \
	../../libs/arduino/CommandParser.cpp \
	../../libs/arduino/Actuator.cpp \
	../../libs/arduino/ActuatorConfigSerializerBase.cpp \
	../../libs/arduino/ActuatorConfigSerializer.cpp \
	../../libs/arduino/ActuatorConfigParser.cpp \
	../../libs/arduino/ActuatorValueSerializerBase.cpp \
	../../libs/arduino/ActuatorValueSerializer.cpp \
	../../libs/arduino/ActuatorValueParser.cpp \

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
