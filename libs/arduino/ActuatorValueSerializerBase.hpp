#ifndef ACTUATORVALUESERIALIZERBASE_HPP
#define ACTUATORVALUESERIALIZERBASE_HPP

#include "ArduMY.hpp"
#include "Converter.hpp"

#include "ActuatorSet.hpp"

#include "ActuatorValuesParserStep.hpp"


struct ActuatorValueSerializerBase {
	ActuatorSet *set;
	ActuatorValuesParserStep step;
	uint16_t byteIndex;
	Converter converter;

	ActuatorValueSerializerBase();

	void setSet(ActuatorSet &set);
	virtual void reset();
	void nextParseStep();


};



#endif // ACTUATORVALUESERIALIZERBASE_HPP
