#ifndef SERVOCONMFIGPARSER_HPP
#define SERVOCONMFIGPARSER_HPP

#include "ActuatorConfigSerializerBase.hpp"

struct ActuatorConfigParser: public ActuatorConfigSerializerBase {
	ActuatorConfigParser();

	bool parse(const uint8_t in);
};


#endif // SERVOCONMFIGPARSER_HPP
