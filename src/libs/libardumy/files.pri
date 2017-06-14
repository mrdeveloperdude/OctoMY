
ARDUMY_TOP= $$SRCS/libs/arduino
ARDUMY_HEADERS += \
	$$ARDUMY_TOP/ArduMYActuatorConfig.hpp \
	$$ARDUMY_TOP/ArduMYActuatorConfigParser.hpp \
	$$ARDUMY_TOP/ArduMYActuatorConfigSerializerBase.hpp \
	$$ARDUMY_TOP/ArduMYActuatorConfigSerializer.hpp \
	$$ARDUMY_TOP/ArduMYActuator.hpp \
	$$ARDUMY_TOP/ArduMYActuatorSet.hpp \
	$$ARDUMY_TOP/ArduMYActuatorState.hpp \
	$$ARDUMY_TOP/ArduMYActuatorValue.hpp \
	$$ARDUMY_TOP/ArduMYActuatorValueParser.hpp \
	$$ARDUMY_TOP/ArduMYActuatorValueRepresentation.hpp \
	$$ARDUMY_TOP/ArduMYActuatorValueSerializerBase.hpp \
	$$ARDUMY_TOP/ArduMYActuatorValueSerializer.hpp \
	$$ARDUMY_TOP/ArduMYCommandParser.hpp \
	$$ARDUMY_TOP/ArduMYCommandSerializer.hpp \
	$$ARDUMY_TOP/MagicDetector.hpp \



ARDUMY_SOURCES += \
	$$ARDUMY_TOP/ArduMYActuatorConfig.cpp \
	$$ARDUMY_TOP/ArduMYActuatorConfigParser.cpp \
	$$ARDUMY_TOP/ArduMYActuatorConfigSerializerBase.cpp \
	$$ARDUMY_TOP/ArduMYActuatorConfigSerializer.cpp \
	$$ARDUMY_TOP/ArduMYActuator.cpp \
	$$ARDUMY_TOP/ArduMYActuatorSet.cpp \
	$$ARDUMY_TOP/ArduMYActuatorState.cpp \
	$$ARDUMY_TOP/ArduMYActuatorValue.cpp \
	$$ARDUMY_TOP/ArduMYActuatorValueParser.cpp \
	$$ARDUMY_TOP/ArduMYActuatorValueRepresentation.cpp \
	$$ARDUMY_TOP/ArduMYActuatorValueSerializerBase.cpp \
	$$ARDUMY_TOP/ArduMYActuatorValueSerializer.cpp \
	$$ARDUMY_TOP/ArduMYCommandParser.cpp \
	$$ARDUMY_TOP/ArduMYCommandSerializer.cpp \
	$$ARDUMY_TOP/MagicDetector.cpp \

HEADERS += $$ARDUMY_HEADERS
SOURCES += $$ARDUMY_SOURCES


