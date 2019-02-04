#include "ardumy_arduino/ArduMY.hpp"

#ifdef OCTOMY_ARDUINO_TARGET



QDebugOutput &operator<<(QDebugOutput &d, const char *str)
{
	Serial.print("QDEBUG: ");
	Serial.println(str);
	return d;
}

QDebugOutput *QDebugOutput::d=nullptr;
QDebugOutput *QDebugOutput::w=nullptr;

#endif // OCTOMY_ARDUINO_TARGET
