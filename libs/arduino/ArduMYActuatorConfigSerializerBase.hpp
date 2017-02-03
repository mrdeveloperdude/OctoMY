#ifndef ACTUATORCONFIGSERIALIZERBASE_HPP
#define ACTUATORCONFIGSERIALIZERBASE_HPP

#include "ArduMY.hpp"
#include "Converter.hpp"

#include "ArduMYActuator.hpp"

#include "ArduMYActuatorConfigParserStep.hpp"


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
