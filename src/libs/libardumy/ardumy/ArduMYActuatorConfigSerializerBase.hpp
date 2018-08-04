#ifndef ACTUATORCONFIGSERIALIZERBASE_HPP
#define ACTUATORCONFIGSERIALIZERBASE_HPP

#include "ardumy/ArduMY.hpp"
#include "Converter.hpp"

#include "ardumy/ArduMYActuator.hpp"

#include "ardumy/ArduMYActuatorConfigParserStep.hpp"


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
