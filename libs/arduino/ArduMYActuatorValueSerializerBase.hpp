#ifndef ACTUATORVALUESERIALIZERBASE_HPP
#define ACTUATORVALUESERIALIZERBASE_HPP

#include "ArduMY.hpp"
#include "Converter.hpp"

#include "ArduMYActuatorSet.hpp"

#include "ArduMYActuatorValuesParserStep.hpp"


struct ArduMYActuatorValueSerializerBase {

public:
	ArduMYActuatorSet *set;
	ActuatorValuesParserStep step;
	uint16_t byteIndex;

	int16_t currentActuatorIndex;
	ArduMYActuatorValueRepresentation currentBatchRepresentation;
	static const uint16_t actuatorMaxCount=256;
	uint8_t enabledActuatorCount;
	uint8_t enableBits[32];

	Converter converter;

public:

	ArduMYActuatorValueSerializerBase();


public:
	void setSet(ArduMYActuatorSet &set);
	virtual void reset();
	void nextParseStep();

	bool isDone() const;

	// Traverse enabled actuators by batch
	void nextActuator();


public:

	void nextBatch();
	ArduMYActuator *currentActuator() const;
	bool currentActuatorIsEnabled() const;
	bool currentActuatorIsOfRepresentation(ArduMYActuatorValueRepresentation) const;





};



#endif // ACTUATORVALUESERIALIZERBASE_HPP
