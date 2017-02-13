#ifndef ARDUINOPIN_HPP
#define ARDUINOPIN_HPP


#include "../../libs/arduino/ArduMYActuatorFlags.hpp"


#include <QString>
#include <QDebug>


struct ArduinoPin {

	uint8_t avrIndex;
	QString avrName;
	uint8_t ardIndex;
	QString ardName;

	uint8_t analogInputBits; // Analogue precision (when analogue is available, see flags)
	uint8_t pwmBits; // PWM precision (when PWM is available, see flags)

	uint8_t flags;

	ArduinoPin()
		: avrIndex(0)
		, avrName()
		, ardIndex(0)
		, ardName()
		, analogInputBits(0)
		, pwmBits(0)
		, flags(0)
	{
		// default constructed is not valid
		setvalid(false);
	}

	ArduinoPin(uint8_t avi, QString avn="", uint8_t ari=0, QString arn="", uint8_t ab=0, uint8_t pb=0, bool av=false, bool dgi=false, bool dgo=false)
		: avrIndex(avi)
		, avrName(avn)
		, ardIndex(ari)
		, ardName(arn)
		, analogInputBits(ab)
		, pwmBits(pb)
		, flags(0)
	{
		setvalid(true);
		setAvailable(av);
		setSupportsDigitalInput(dgi);
		setSupportsDigitalOutput(dgo);
		//	qDebug()<<"CTOR ";
	}

	ARDUMY_ACTUATOR_FLAG_SELECTOR(valid,				setvalid,				0 ) // This pin data is valid and can be trusted
	ARDUMY_ACTUATOR_FLAG_SELECTOR(available,				setAvailable,				1 ) // This pin is available for use
	ARDUMY_ACTUATOR_FLAG_SELECTOR(supportsDigitalInput,		setSupportsDigitalInput,	2 ) // This pin can support digital input
	ARDUMY_ACTUATOR_FLAG_SELECTOR(supportsDigitalOutput,	setSupportsDigitalOutput,	3 ) // This pin can support digital output

	void configure(uint8_t avi, QString avn, uint8_t ari, QString arn, uint8_t ab, uint8_t pb, bool av, bool dgi, bool dgo)
	{
		setvalid(true);
		avrIndex=avi;
		avrName=avn;
		ardIndex=ari;
		ardName=arn;
		analogInputBits=ab;
		pwmBits=pb;
		setAvailable(av);
		setSupportsDigitalInput(dgi);
		setSupportsDigitalOutput(dgo);
	}


	inline bool supportsPWM()
	{
		return pwmBits>0;
	}

	inline bool supportsAnalogInput()
	{
		return analogInputBits>0;
	}

	QString fullDescription()
	{
		if(!valid()) {
			return "NOT VALID";
		}
		QString out("avr=%1 ard=%2 %3.%4.%5.%6 %7 ( %8 )");
		return out.arg(avrIndex,2,10,  QChar(' ')).arg(ardIndex,2,10,  QChar(' ')).arg(supportsDigitalInput()?"I":"_").arg(supportsDigitalOutput()?"O":"_").arg(supportsAnalogInput()?"A":"_").arg(supportsPWM()?"P":"_").arg(ardName).arg(avrName);
	}

};

#endif // ARDUINOPIN_HPP
