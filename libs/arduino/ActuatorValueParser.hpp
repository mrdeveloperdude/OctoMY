#ifndef SETSERVOPOSITIONSSTATE_HPP
#define SETSERVOPOSITIONSSTATE_HPP


#include "ActuatorValueSerializerBase.hpp"


struct ActuatorValueParser: public ActuatorValueSerializerBase {
public:

public:
	ActuatorValueParser();


public:


	void nextActuator();

	bool parse(const uint8_t in);


	bool isDone() const;
	// Look at enabled bits and return the actual servo index by skipping disabled servos
//	uint8_t enabledServoByIndex(uint8_t in);
};


#endif // SETSERVOPOSITIONSSTATE_HPP
