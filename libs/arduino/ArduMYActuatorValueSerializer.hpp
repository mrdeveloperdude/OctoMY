#ifndef ACTUATORVALUESERIALIZER_HPP
#define ACTUATORVALUESERIALIZER_HPP

#include "ArduMYActuatorValueSerializerBase.hpp"


struct ArduMYActuatorValueSerializer: public ArduMYActuatorValueSerializerBase {
public:
	uint8_t representationByteCount;
	uint8_t enableByteIndex;
public:
	ArduMYActuatorValueSerializer();

public:

	void reset() override;

	// Return true if there is more data remaining to be read
	bool hasMoreData() const;

	// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
	uint8_t nextByte();


};


#endif // ACTUATORVALUESERIALIZER_HPP
