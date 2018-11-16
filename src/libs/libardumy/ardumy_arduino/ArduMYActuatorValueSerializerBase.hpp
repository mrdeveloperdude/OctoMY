#ifndef ACTUATORVALUESERIALIZERBASE_HPP
#define ACTUATORVALUESERIALIZERBASE_HPP

#include "ardumy_arduino/ArduMY.hpp"
#include "ardumy_arduino/Converter.hpp"

#include "ardumy_arduino/ArduMYActuatorSet.hpp"

#include "ardumy_arduino/ArduMYActuatorValuesParserStep.hpp"


struct ArduMYActuatorValueSerializerBase {

public:
	ArduMYActuatorSet *set;
	ArduMYActuatorValuesParserStep step;
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
