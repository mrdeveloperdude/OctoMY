#ifndef TEST_INC_HPP
#define TEST_INC_HPP


#include "../arduino/ArduMYActuatorConfigParser.hpp"
#include "../arduino/ArduMYActuatorConfigSerializer.hpp"


#include "../arduino/ArduMYActuatorValueParser.hpp"
#include "../arduino/ArduMYActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QTest>

#include "../libcore/hw/controllers/ardumy/ArdumyTypeConversions.hpp"

void logLines(const QString ret);


#endif // TEST_INC_HPP
