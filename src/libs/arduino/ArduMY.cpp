#include "ArduMY.hpp"

#ifdef ARDUINO_ARCH_AVR



QDebugOutput &operator<<(QDebugOutput &d, const char *str)
{
	Serial.print("QDEBUG: ");
	Serial.println(str);
	return d;
}

QDebugOutput *QDebugOutput::d=nullptr;
QDebugOutput *QDebugOutput::w=nullptr;

#endif
