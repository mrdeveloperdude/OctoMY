
ARDUMY_TOP= $${SRCS}/libs/libardumy
ARDUMY_HEADERS += \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfig.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfigParser.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfigSerializerBase.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfigSerializer.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuator.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorSet.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorState.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValue.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueParser.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueRepresentation.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueSerializerBase.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueSerializer.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYCommandParser.hpp \
	$$ARDUMY_TOP/ardumy/ArduMYCommandSerializer.hpp \
	$$ARDUMY_TOP/ardumy/MagicDetector.hpp \
	$$ARDUMY_TOP/ardumy/FivePhase.hpp \



ARDUMY_SOURCES += \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfig.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfigParser.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfigSerializerBase.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorConfigSerializer.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuator.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorSet.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorState.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValue.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueParser.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueRepresentation.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueSerializerBase.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYActuatorValueSerializer.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYCommandParser.cpp \
	$$ARDUMY_TOP/ardumy/ArduMYCommandSerializer.cpp \
	$$ARDUMY_TOP/ardumy/MagicDetector.cpp \
	$$ARDUMY_TOP/ardumy/FivePhase.cpp \

HEADERS += $$ARDUMY_HEADERS \

SOURCES += $$ARDUMY_SOURCES \



