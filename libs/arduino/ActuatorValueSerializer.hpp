#ifndef ACTUATORVALUESERIALIZER_HPP
#define ACTUATORVALUESERIALIZER_HPP

#include "ActuatorValueSerializerBase.hpp"


struct ActuatorValueSerializer: public ActuatorValueSerializerBase {

public:

	uint8_t actuatorIndex;
public:
	ActuatorValueSerializer();

public:

	void nextActuator();

	// Return true if there is more data remaining to be read
	bool hasMoreData() const;

	// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
	uint8_t nextByte();


};


#endif // ACTUATORVALUESERIALIZER_HPP
