
contains(DEFINES, ARDUINO_ARCH_AVR){

SOURCES += \
	ArduMYActuatorConfig.cpp \
	ArduMYActuatorConfigParser.cpp \
	ArduMYActuatorConfigParserStep.cpp \
	ArduMYActuatorConfigSerializerBase.cpp \
	ArduMYActuatorConfigSerializer.cpp \
	ArduMYActuator.cpp \
	ArduMYActuatorSet.cpp \
	ArduMYActuatorState.cpp \
	ArduMYActuatorType.cpp \
	ArduMYActuatorValue.cpp \
	ArduMYActuatorValueParser.cpp \
	ArduMYActuatorValueRepresentation.cpp \
	ArduMYActuatorValueSerializerBase.cpp \
	ArduMYActuatorValueSerializer.cpp \
	ArduMYActuatorValuesParserStep.cpp \
	ArduMYBuzzerActuator.cpp \
	CommandParser.cpp \
	CommandSerializer.cpp \
	Converter.cpp \
	DynamicArray.cpp \
	MagicDetector.cpp \
	main.cpp \
	ParserState.cpp \
	RCServoActuator.cpp \
	RGBLED.cpp \
	SetServoPositionsParser.cpp \
	StepMotorActuator.cpp \
	SwitchInput.cpp \



HEADERS += \
	ArduMYActuatorConfig.hpp \
	ArduMYActuatorConfigParser.hpp \
	ArduMYActuatorConfigParserStep.hpp \
	ArduMYActuatorConfigSerializerBase.hpp \
	ArduMYActuatorConfigSerializer.hpp \
	ArduMYActuatorFlags.hpp \
	ArduMYActuator.hpp \
	ArduMYActuatorSet.hpp \
	ArduMYActuatorState.hpp \
	ArduMYActuatorType.hpp \
	ArduMYActuatorValue.hpp \
	ArduMYActuatorValueParser.hpp \
	ArduMYActuatorValueRepresentation.hpp \
	ArduMYActuatorValueSerializerBase.hpp \
	ArduMYActuatorValueSerializer.hpp \
	ArduMYActuatorValuesParserStep.hpp \
	ArduMYBuzzerActuator.hpp \
	ArduMY.hpp \
	CommandParser.hpp \
	CommandSerializer.hpp \
	Converter.hpp \
	DynamicArray.hpp \
	MagicDetector.hpp \
	ParserState.hpp \
	RCServoActuator.hpp \
	RGBLED.hpp \
	SetServoPositionsParser.hpp \
	StepMotorActuator.hpp \
	SwitchInput.hpp \




DISTFILES += \
	Makefile \

ARDUINO_PATH=/home/lennart/ArduMY/arduino-1.8.0

INCLUDEPATH = $${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino \
#$${ARDUINO_PATH}/libraries/Bridge/src \
#$${ARDUINO_PATH}/libraries/Esplora/src \
#$${ARDUINO_PATH}/libraries/Ethernet/src \
#$${ARDUINO_PATH}/libraries/Firmata/src \
#$${ARDUINO_PATH}/libraries/GSM/src \
#$${ARDUINO_PATH}/libraries/LiquidCrystal/src \
#$${ARDUINO_PATH}/libraries/Robot_Control/src \
#$${ARDUINO_PATH}/libraries/RobotIRremote/src \
#$${ARDUINO_PATH}/libraries/Robot_Motor/src \
#$${ARDUINO_PATH}/libraries/SD/src \
$${ARDUINO_PATH}/libraries/Servo/src \
#$${ARDUINO_PATH}/libraries/SpacebrewYun/src \
$${ARDUINO_PATH}/libraries/Stepper/src \
#$${ARDUINO_PATH}/libraries/Temboo/src \
#$${ARDUINO_PATH}/libraries/TFT/src \
#$${ARDUINO_PATH}/libraries/WiFi/src \
$${ARDUINO_PATH}/libraries/AccelStepper \
#$${ARDUINO_PATH}/hardware/arduino/avr/libraries/EEPROM \
$${ARDUINO_PATH}/hardware/arduino/avr/libraries/SoftwareSerial \
$${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI \
$${ARDUINO_PATH}/hardware/arduino/avr/libraries/Wire \



DEFINES += USE_STATUS
include($$TOP_PWD/common.pri)
DEFINES -= USE_STATUS

}

SOURCES += \
    ArduMY.cpp \
    BoardInfo.cpp \
    ArduMYMain.cpp \
    ArduMYActuatorEnableBits.cpp

HEADERS += \
    BoardInfo.hpp \
    ArduMYMain.hpp \
    ArduMYActuatorEnableBits.hpp





