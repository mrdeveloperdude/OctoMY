#ifndef ACTUATORCONFIGSERIALIZER_HPP
#define ACTUATORCONFIGSERIALIZER_HPP

#include "ArduMYActuatorConfigSerializerBase.hpp"

struct ArduMYActuatorConfigSerializer: public ArduMYActuatorConfigSerializerBase {


	ArduMYActuatorConfigSerializer();

	// Return true if there is more data remaining to be read
	bool hasMoreData() const;

	// Get the next byte of serial data. NOTE: only valid when hasMoreData() returns true, if not will return 0x00
	uint8_t nextByte();


};

#endif // ACTUATORCONFIGSERIALIZER_HPP
