#ifndef ACTUATORCONFIGSERIALIZERBASE_HPP
#define ACTUATORCONFIGSERIALIZERBASE_HPP

#include "ardumy_arduino/ArduMY.hpp"
#include "ardumy_arduino/Converter.hpp"

#include "ardumy_arduino/ArduMYActuator.hpp"

#include "ardumy_arduino/ArduMYActuatorConfigParserStep.hpp"


struct ArduMYActuatorConfigSerializerBase {
	ArduMYActuatorConfig *config;
	ArduMYActuatorConfigParserStep step;
	uint16_t byteIndex;
	Converter converter;

	ArduMYActuatorConfigSerializerBase();

	void setConfig(ArduMYActuatorConfig &config);
	void reset();
	void nextStep();


};

#endif // ACTUATORCONFIGSERIALIZERBASE_HPP
