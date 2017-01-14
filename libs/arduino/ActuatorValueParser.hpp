#ifndef SETSERVOPOSITIONSSTATE_HPP
#define SETSERVOPOSITIONSSTATE_HPP


#include "ActuatorValueSerializerBase.hpp"


struct ActuatorValueParser: public ActuatorValueSerializerBase {
public:
	int16_t currentActuatorIndex;
	ActuatorValueRepresentation currentBatchRepresentation;
	static const uint16_t actuatorMaxCount=256;
	uint8_t enabledActuatorCount;
	uint8_t enableBits[32];

public:
	ActuatorValueParser();

public:
	bool isDone() const;
	Actuator *currentActuator() const;
	bool currentActuatorIsEnabled() const;
	bool currentActuatorIsOfRepresentation(ActuatorValueRepresentation) const;


public:

	void nextBatch();
	void nextActuator();
	void reset() override;
	bool parse(const uint8_t in);


	// Look at enabled bits and return the actual servo index by skipping disabled servos
//	uint8_t enabledServoByIndex(uint8_t in);
};


#endif // SETSERVOPOSITIONSSTATE_HPP
