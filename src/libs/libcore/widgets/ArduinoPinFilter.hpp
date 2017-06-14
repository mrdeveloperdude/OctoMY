#ifndef ARDUINOPINFILTER_HPP
#define ARDUINOPINFILTER_HPP

#include <qglobal.h>

enum ArduinoPinType: quint8 {
	PIN_DIGITAL_INPUT, PIN_DIGITAL_OUTPUT, PIN_ANALOG_INPUT, PIN_PWM_OUTPUT
};

class ArduinoPinFilter
{
private:
	ArduinoPinType mType;
public:
	// Default to the most common pin type
	explicit ArduinoPinFilter(ArduinoPinType type=PIN_DIGITAL_OUTPUT);
public:
	ArduinoPinType type()
	{
		return mType;
	}

};

#endif // ARDUINOPINFILTER_HPP
