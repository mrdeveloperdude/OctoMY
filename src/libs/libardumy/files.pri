
# Please note, the ardumy lib contains sources targetting the actual binary on arduino hardware as sell as the client library that is part of OctoMY node.
# The list of files below represent the client library, to build actual arduino targets, see separate subproject/documentation

# Inspiration from :
#  +  https://blog.jayway.com/2011/09/22/using-qtcreator-for-arduino-development/
#  +  https://hackaday.com/2015/10/01/arduino-development-theres-a-makefile-for-that/


# These files can run on arduino targets as well as be included in OctoMY Qt5 build
ARDUMY_ARDUINO_TOP=$${SRCS}/libs/libardumy/ardumy_arduino


ARDUMY_ARDUINO_HEADERS += \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfig.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfigParser.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfigSerializerBase.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfigSerializer.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuator.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorSet.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorState.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValue.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueParser.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueRepresentation.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueSerializerBase.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueSerializer.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYCommandParser.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYCommandSerializer.hpp \
	$$ARDUMY_ARDUINO_TOP/BoardInfo.hpp \
	$$ARDUMY_ARDUINO_TOP/MagicDetector.hpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYMain.hpp \
	$$ARDUMY_ARDUINO_TOP/Converter.hpp \
	$$ARDUMY_ARDUINO_TOP/DynamicArray.hpp \


ARDUMY_ARDUINO_SOURCES += \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfig.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfigParser.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfigSerializerBase.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorConfigSerializer.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuator.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorSet.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorState.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValue.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueParser.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueRepresentation.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueSerializerBase.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYActuatorValueSerializer.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYCommandParser.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYCommandSerializer.cpp \
	$$ARDUMY_ARDUINO_TOP/BoardInfo.cpp \
	$$ARDUMY_ARDUINO_TOP/MagicDetector.cpp \
	$$ARDUMY_ARDUINO_TOP/ArduMYMain.cpp \
	$$ARDUMY_ARDUINO_TOP/Converter.cpp \
	$$ARDUMY_ARDUINO_TOP/DynamicArray.cpp \


# These files can not run on arduino targets and are only useful to OctoMY Qt5 build
ARDUMY_TOP=$${SRCS}/libs/libardumy/ardumy


ARDUMY_HEADERS += \
	$$ARDUMY_TOP/ArduMYActuatorEnableBits.hpp \
	$$ARDUMY_TOP/ArduMYBuzzerActuator.hpp \
	$$ARDUMY_TOP/FivePhase.hpp \
	$$ARDUMY_TOP/RCServoActuator.hpp \
	$$ARDUMY_TOP/RGBLED.hpp \
	$$ARDUMY_TOP/StepMotorActuator.hpp \
	$$ARDUMY_TOP/SwitchInput.hpp \

ARDUMY_SOURCES += \
	$$ARDUMY_TOP/ArduMYActuatorEnableBits.cpp \
	$$ARDUMY_TOP/ArduMYBuzzerActuator.cpp \
	$$ARDUMY_TOP/FivePhase.cpp \
	$$ARDUMY_TOP/RCServoActuator.cpp \
	$$ARDUMY_TOP/RGBLED.cpp \
	$$ARDUMY_TOP/StepMotorActuator.cpp \
	$$ARDUMY_TOP/SwitchInput.cpp \

HEADERS += \
	$$ARDUMY_ARDUINO_HEADERS \
	$$ARDUMY_HEADERS \

SOURCES += \
	$$ARDUMY_ARDUINO_SOURCES \
	$$ARDUMY_SOURCES \



