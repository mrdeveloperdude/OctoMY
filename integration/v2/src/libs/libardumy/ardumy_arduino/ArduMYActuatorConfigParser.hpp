#ifndef SERVOCONMFIGPARSER_HPP
#define SERVOCONMFIGPARSER_HPP

#include "ardumy_arduino/ArduMYActuatorConfigSerializerBase.hpp"

struct ArduMYActuatorConfigParser: public ArduMYActuatorConfigSerializerBase {
	ArduMYActuatorConfigParser();

	bool parse(const uint8_t in);
};


#endif // SERVOCONMFIGPARSER_HPP
