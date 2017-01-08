#ifndef ACTUATORCONFIGSERIALIZERBASE_HPP
#define ACTUATORCONFIGSERIALIZERBASE_HPP

#include "ArduMY.hpp"
#include "Converter.hpp"

#include "Actuator.hpp"

#include "ActuatorConfigParserStep.hpp"


struct ActuatorConfigSerializerBase {
	ActuatorConfig *config;
	ActuatorConfigParserStep step;
	uint16_t byteIndex;
	Converter converter;

	ActuatorConfigSerializerBase();

	void setConfig(ActuatorConfig &config);
	void reset();
	void nextStep();


};

#endif // ACTUATORCONFIGSERIALIZERBASE_HPP
