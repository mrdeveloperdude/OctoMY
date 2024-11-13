#ifndef SETSERVOPOSITIONSSTATE_HPP
#define SETSERVOPOSITIONSSTATE_HPP

#include "ardumy_arduino/serializer/ArduMYActuatorValueSerializerBase.hpp"

struct ArduMYActuatorValueParser : public ArduMYActuatorValueSerializerBase
{
public:

public:
	ArduMYActuatorValueParser();

public:
	bool parse(const uint8_t in);
};

#endif // SETSERVOPOSITIONSSTATE_HPP
