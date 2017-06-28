#ifndef TEST_INC_HPP
#define TEST_INC_HPP


#include "../arduino/ArduMYActuatorConfigParser.hpp"
#include "../arduino/ArduMYActuatorConfigSerializer.hpp"


#include "../arduino/ArduMYActuatorValueParser.hpp"
#include "../arduino/ArduMYActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/ArduMYCommandParser.hpp"

#include "../arduino/ArduMYParserState.hpp"

#include <QTest>

#include "hw/controllers/ardumy/ArduMYTypeConversions.hpp"

void logLines(const QString ret);


#endif // TEST_INC_HPP
