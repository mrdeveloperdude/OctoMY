include($$TESTS/test.pri)

TARGET = test_ardumy

ARDUMY_BASE=$$SRCS/libs/arduino

HEADERS += \
	$$ARDUMY_BASE/ArduMYActuatorConfig.hpp \
	$$ARDUMY_BASE/ArduMYActuatorConfigParser.hpp \
	$$ARDUMY_BASE/ArduMYActuatorConfigSerializerBase.hpp \
	$$ARDUMY_BASE/ArduMYActuatorConfigSerializer.hpp \
	$$ARDUMY_BASE/ArduMYActuator.hpp \
	$$ARDUMY_BASE/ArduMYActuatorSet.hpp \
	$$ARDUMY_BASE/ArduMYActuatorState.hpp \
	$$ARDUMY_BASE/ArduMYActuatorValue.hpp \
	$$ARDUMY_BASE/ArduMYActuatorValueParser.hpp \
	$$ARDUMY_BASE/ArduMYActuatorValueRepresentation.hpp \
	$$ARDUMY_BASE/ArduMYActuatorValueSerializerBase.hpp \
	$$ARDUMY_BASE/ArduMYActuatorValueSerializer.hpp \
	$$ARDUMY_BASE/ArduMYCommandParser.hpp \
	$$ARDUMY_BASE/ArduMYCommandSerializer.hpp \
	$$ARDUMY_BASE/MagicDetector.hpp \
	testActuatorConfigEquals.inc.hpp \
	testActuatorConfigParser.inc.hpp \
	testActuatorConfigSerializer.inc.hpp \
	testActuatorValueEquals.inc.hpp \
	testActuatorValueParser.inc.hpp \
	testActuatorValueSerializer.inc.hpp \
	TestArduMY.hpp \
	test.inc.hpp \



SOURCES += \
	$$ARDUMY_BASE/ArduMYActuatorConfig.cpp \
	$$ARDUMY_BASE/ArduMYActuatorConfigParser.cpp \
	$$ARDUMY_BASE/ArduMYActuatorConfigSerializerBase.cpp \
	$$ARDUMY_BASE/ArduMYActuatorConfigSerializer.cpp \
	$$ARDUMY_BASE/ArduMYActuator.cpp \
	$$ARDUMY_BASE/ArduMYActuatorSet.cpp \
	$$ARDUMY_BASE/ArduMYActuatorState.cpp \
	$$ARDUMY_BASE/ArduMYActuatorValue.cpp \
	$$ARDUMY_BASE/ArduMYActuatorValueParser.cpp \
	$$ARDUMY_BASE/ArduMYActuatorValueRepresentation.cpp \
	$$ARDUMY_BASE/ArduMYActuatorValueSerializerBase.cpp \
	$$ARDUMY_BASE/ArduMYActuatorValueSerializer.cpp \
	$$ARDUMY_BASE/ArduMYCommandParser.cpp \
	$$ARDUMY_BASE/ArduMYCommandSerializer.cpp \
	$$ARDUMY_BASE/MagicDetector.cpp \
	TestArduMY.cpp \

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
