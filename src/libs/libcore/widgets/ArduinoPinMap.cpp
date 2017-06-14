#include "ArduinoPinMap.hpp"



#include <QDebug>

ArduinoPinMap::ArduinoPinMap()
{

}

#define TRUEE true

ArduinoPinMap ArduinoPinMap::pinMapFactory(QString boardName)
{
	ArduinoPinMap map;
	// Reference: https://www.arduino.cc/en/Hacking/PinMapping2560
	// Reference: https://lynx2015.files.wordpress.com/2015/08/arduino-mega-pinout-diagram.png
	if(boardName.trimmed().toLower()=="mega2560") {
		map.mPins.reserve(70);
		//						avIndex,	avName,	arIndex,	arName,					aBits,	pwBits,	available,	digIn,	digOut,	PWM,	anIn
		map.mPins << ArduinoPin(2,			"PE0",	0,			"Digital pin 0 (RX0)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(3,			"PE1",	1,			"Digital pin1 (TX0)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(6,			"PE4",	2,			"Digital pin2 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(7,			"PE5",	3,			"Digital pin3 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(1,			"PG5",	4,			"Digital pin4 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(5,			"PE3",	5,			"Digital pin5 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(6,			"PE4",	6,			"Digital pin6 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );// TODO: Which one of these can be put into 16 bit PWM? And will we bother?
		map.mPins << ArduinoPin(16,			"PH4",	7,			"Digital pin7 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(17,			"PH5",	8,			"Digital pin8 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(18,			"PH6",	9,			"Digital pin9 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(23,			"PB4",	10,			"Digital pin10 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(24,			"PB5",	11,			"Digital pin11 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(25,			"PB6",	12,			"Digital pin12 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(26,			"PB7",	13,			"Digital pin13 (PWM)",	0,		8,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(64,			"PJ1",	14,			"Digital pin14 (TX3)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(63,			"PJ0",	15,			"Digital pin15 (RX3)",	0,		0,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(13,			"PH1",	16,			"Digital pin16 (TX2)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(12,			"PH0",	17,			"Digital pin17 (RX2)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(46,			"PD3",	18,			"Digital pin18 (TX1)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(45,			"PD2",	19,			"Digital pin19 (RX1)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(44,			"PD1",	20,			"Digital pin20 (SDA)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(43,			"PD0",	21,			"Digital pin21 (SCL)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(78,			"PA0",	22,			"Digital pin22",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(77,			"PA1",	23,			"Digital pin23",		0,		0,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(76,			"PA2",	24,			"Digital pin24",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(75,			"PA3",	25,			"Digital pin25",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(74,			"PA4",	26,			"Digital pin26",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(73,			"PA5",	27,			"Digital pin27",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(72,			"PA6",	28,			"Digital pin28",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(71,			"PA7",	29,			"Digital pin29",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(60,			"PC7",	30,			"Digital pin30",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(59,			"PC6",	31,			"Digital pin31",		0,		0,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(58,			"PC5",	32,			"Digital pin32",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(57,			"PC4",	33,			"Digital pin33",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(56,			"PC3",	34,			"Digital pin34",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(55,			"PC2",	35,			"Digital pin35",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(54,			"PC1",	36,			"Digital pin36",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(53,			"PC0",	37,			"Digital pin37",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(38,			"PL3",	38,			"Digital pin46 (PWM)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(39,			"PL4",	39,			"Digital pin45 (PWM)",	0,		0,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(52,			"PG1",	40,			"Digital pin40",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(51,			"PG0",	41,			"Digital pin41",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(42,			"PL7",	42,			"Digital pin42",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(41,			"PL6",	43,			"Digital pin43 (PWM)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(40,			"PL5",	44,			"Digital pin44 (PWM)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(39,			"PL4",	45,			"Digital pin45 (PWM)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(38,			"PL3",	46,			"Digital pin46 (PWM)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(37,			"PL2",	47,			"Digital pin47",		0,		0,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(36,			"PL1",	48,			"Digital pin48",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(35,			"PL0",	49,			"Digital pin49",		0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(22,			"PB3",	50,			"Digital pin50 (MISO)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(21,			"PB2",	51,			"Digital pin51 (MOSI)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(20,			"PB1",	52,			"Digital pin52 (SCK)",	0,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(19,			"PB0",	53,			"Digital pin53 (SS)",	0,		0,		false,		TRUEE,	TRUEE );

		// https://www.arduino.cc/en/Tutorial/AnalogInputPins
		map.mPins << ArduinoPin(97,			"PF0",	54,			"Analog pin0",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(96,			"PF1",	55,			"Analog pin1",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(95,			"PF2",	56,			"Analog pin2",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(94,			"PF3",	57,			"Analog pin3",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(93,			"PF4",	58,			"Analog pin4",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(92,			"PF5",	59,			"Analog pin5",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(91,			"PF6",	60,			"Analog pin6",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(90,			"PF7",	61,			"Analog pin7",			10,		0,		false,		TRUEE,	TRUEE );

		map.mPins << ArduinoPin(89,			"PK0",	62,			"Analog pin8",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(88,			"PK1",	63,			"Analog pin9",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(87,			"PK2",	64,			"Analog pin10",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(86,			"PK3",	65,			"Analog pin11",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(85,			"PK4",	66,			"Analog pin12",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(84,			"PK5",	67,			"Analog pin13",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(83,			"PK6",	68,			"Analog pin14",			10,		0,		false,		TRUEE,	TRUEE );
		map.mPins << ArduinoPin(82,			"PK7",	69,			"Analog pin15",			10,		0,		false,		TRUEE,	TRUEE );

	}
	return map;
}



void ArduinoPinMap::verifyIntegrity()
{
	int  sz=mPins.size();
	for(int i=0; i<sz; ++i) {
		int j=0;
		for(j=0; j<sz; ++j) {
			ArduinoPin &pin=mPins[j];
			if(pin.ardIndex==i) {
				break;
			}
		}
		if(i==j) {
			qWarning()<<"MISSED: "<<i;
		} else {
			qDebug()<<"GOT: "<<i;
		}
	}
}


QVector<ArduinoPin *> ArduinoPinMap::filter(ArduinoPinFilter &f)
{
	QVector<ArduinoPin *> out;
	return out;
}

QStringList ArduinoPinMap::pinStringList()
{
	QStringList strings;
	int  sz=mPins.size();
	for(int i=0; i<sz; ++i) {
		ArduinoPin &pin=mPins[i];
		strings << pin.fullDescription();
	}
	return strings;
}

ArduinoPin ArduinoPinMap::pin(quint32 idx)
{
	if(idx>=mPins.size()) {
		return ArduinoPin();
	}
	return mPins[idx];
}
