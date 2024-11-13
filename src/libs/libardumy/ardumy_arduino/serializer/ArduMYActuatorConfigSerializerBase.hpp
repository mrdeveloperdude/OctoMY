#ifndef ACTUATORCONFIGSERIALIZERBASE_HPP
#define ACTUATORCONFIGSERIALIZERBASE_HPP

#include "ardumy_arduino/utils/Converter.hpp"

#include "ardumy_arduino/parser/ArduMYActuatorConfigParserStep.hpp"


class ArduMYActuatorConfig;

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
