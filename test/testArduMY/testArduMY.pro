include($$TOP_PWD/test/test.pri)

TARGET = test_ardumy

HEADERS += \
	../../libs/arduino/ArduMYActuatorConfig.hpp \
	../../libs/arduino/ArduMYActuatorConfigParser.hpp \
	../../libs/arduino/ArduMYActuatorConfigSerializerBase.hpp \
	../../libs/arduino/ArduMYActuatorConfigSerializer.hpp \
	../../libs/arduino/ArduMYActuator.hpp \
	../../libs/arduino/ArduMYActuatorSet.hpp \
	../../libs/arduino/ArduMYActuatorState.hpp \
	../../libs/arduino/ArduMYActuatorValue.hpp \
	../../libs/arduino/ArduMYActuatorValueParser.hpp \
	../../libs/arduino/ArduMYActuatorValueRepresentation.hpp \
	../../libs/arduino/ArduMYActuatorValueSerializerBase.hpp \
	../../libs/arduino/ArduMYActuatorValueSerializer.hpp \
	../../libs/arduino/CommandParser.hpp \
	../../libs/arduino/MagicDetector.hpp \
	testActuatorConfigEquals.inc.hpp \
	testActuatorConfigParser.inc.hpp \
	testActuatorConfigSerializer.inc.hpp \
	testActuatorValueEquals.inc.hpp \
	testActuatorValueParser.inc.hpp \
	testActuatorValueSerializer.inc.hpp \
	TestArduMY.hpp \
	test.inc.hpp \



SOURCES += \
	../../libs/arduino/ArduMYActuatorConfig.cpp \
	../../libs/arduino/ArduMYActuatorConfigParser.cpp \
	../../libs/arduino/ArduMYActuatorConfigSerializerBase.cpp \
	../../libs/arduino/ArduMYActuatorConfigSerializer.cpp \
	../../libs/arduino/ArduMYActuator.cpp \
	../../libs/arduino/ArduMYActuatorSet.cpp \
	../../libs/arduino/ArduMYActuatorState.cpp \
	../../libs/arduino/ArduMYActuatorValue.cpp \
	../../libs/arduino/ArduMYActuatorValueParser.cpp \
	../../libs/arduino/ArduMYActuatorValueRepresentation.cpp \
	../../libs/arduino/ArduMYActuatorValueSerializerBase.cpp \
	../../libs/arduino/ArduMYActuatorValueSerializer.cpp \
	../../libs/arduino/CommandParser.cpp \
	../../libs/arduino/MagicDetector.cpp \
	TestArduMY.cpp \

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
