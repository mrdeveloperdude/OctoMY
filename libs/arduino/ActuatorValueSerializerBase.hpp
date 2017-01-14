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

	int16_t currentActuatorIndex;
	ActuatorValueRepresentation currentBatchRepresentation;
	static const uint16_t actuatorMaxCount=256;
	uint8_t enabledActuatorCount;
	uint8_t enableBits[32];

	Converter converter;

	ActuatorValueSerializerBase();

	void setSet(ActuatorSet &set);
	virtual void reset();
	void nextParseStep();

public:

	void nextBatch();
	Actuator *currentActuator() const;
	bool currentActuatorIsEnabled() const;
	bool currentActuatorIsOfRepresentation(ActuatorValueRepresentation) const;





};



#endif // ACTUATORVALUESERIALIZERBASE_HPP
