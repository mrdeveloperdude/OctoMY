include($$TOP_PWD/test/test.pri)

TARGET = test_ardumy

HEADERS += \
	TestArduMY.hpp \
	../../libs/arduino/MagicDetector.hpp \
	../../libs/arduino/CommandParser.hpp \
	../../libs/arduino/ArduMYActuator.hpp \
	../../libs/arduino/ArduMYActuatorConfig.hpp \
	../../libs/arduino/ArduMYActuatorState.hpp \
	../../libs/arduino/ArduMYActuatorValue.hpp \
	../../libs/arduino/ArduMYActuatorValueRepresentation.hpp \
	../../libs/arduino/ArduMYActuatorSet.hpp \
	../../libs/arduino/ArduMYActuatorConfigSerializerBase.hpp \
	../../libs/arduino/ArduMYActuatorConfigSerializer.hpp \
	../../libs/arduino/ArduMYActuatorConfigParser.hpp \
	../../libs/arduino/ArduMYActuatorValueSerializerBase.hpp \
	../../libs/arduino/ArduMYActuatorValueSerializer.hpp \
	../../libs/arduino/ArduMYActuatorValueParser.hpp \
	testActuatorConfigEquals.inc.hpp \
	testActuatorValueParser.inc.hpp \
	testActuatorConfigParser.inc.hpp \
	testActuatorConfigSerializer.inc.hpp \
	testActuatorValueSerializer.inc.hpp \
	testActuatorValueEquals.inc.hpp \
	test.inc.hpp


SOURCES += \
	TestArduMY.cpp \
	../../libs/arduino/MagicDetector.cpp \
	../../libs/arduino/CommandParser.cpp \
	../../libs/arduino/ArduMYActuator.cpp \
	../../libs/arduino/ArduMYActuatorConfig.cpp \
	../../libs/arduino/ArduMYActuatorState.cpp \
	../../libs/arduino/ArduMYActuatorValue.cpp \
	../../libs/arduino/ArduMYActuatorValueRepresentation.cpp \
	../../libs/arduino/ArduMYActuatorSet.cpp \
	../../libs/arduino/ArduMYActuatorConfigSerializerBase.cpp \
	../../libs/arduino/ArduMYActuatorConfigSerializer.cpp \
	../../libs/arduino/ArduMYActuatorConfigParser.cpp \
	../../libs/arduino/ArduMYActuatorValueSerializerBase.cpp \
	../../libs/arduino/ArduMYActuatorValueSerializer.cpp \
	../../libs/arduino/ArduMYActuatorValueParser.cpp \

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
