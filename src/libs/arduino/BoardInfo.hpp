#ifndef BOARDINFO_HPP
#define BOARDINFO_HPP


#include "ArduMY.hpp"

class BoardInfo
{
public:
	// Arduino SDK Version
	static const uint16_t SDK_VERSION = ARDUINO;

	static const uint8_t MAX_BOARD_NAME_LENGTH = 16;
	static const uint8_t MAX_CPU_NAME_LENGTH = 16;
	char BOARD_NAME[MAX_BOARD_NAME_LENGTH];
	char CPU_NAME[MAX_CPU_NAME_LENGTH];

	static const uint8_t BOARD_UNKNOWN = 0x0;
	static const uint8_t BOARD_UNO = 0x01;
	static const uint8_t BOARD_ZERO = 0x02;
	static const uint8_t BOARD_DUE = 0x03;
	static const uint8_t BOARD_MICRO= 0x04;
	static const uint8_t BOARD_YUN_400 = 0x05;
	static const uint8_t BOARD_LEONARDO = 0x06;
	static const uint8_t BOARD_MEGA = 0x07;
	static const uint8_t BOARD_NANO = 0x08;
	static const uint8_t BOARD_NANO_3 = 0x09;
	static const uint8_t BOARD_LILYPAD = 0x0a;
	static const uint8_t BOARD_LILYPAD_2 = 0x0b;
	static const uint8_t BOARD_TRINKET = 0x0c;
	static const uint8_t BOARD_101 = 0x0d;

	static const uint8_t NUM_FEATURES = 1;
	static const uint8_t FEATURE_MULTIPLE_SERIAL = 0x00;
	static const uint8_t FEATURE_BLUETOOTH_4 = 0x01;
	static const uint8_t FEATURE_ACCELEROMETER = 0x02;
	static const uint8_t FEATURE_GYROSCOPE = 0x03;
	static const uint8_t FEATURE_ANALOG_OUT = 0x04;

	static const unsigned long MAX_MHZ = F_CPU;

	bool FEATURES[NUM_FEATURES];

	// Arduino UNO / Arduino FIO
#if defined(__AVR_ATmega328P__)
	// Board ID
	static const uint8_t BOARD = 0x01;
	// CPU architecture bits
	static const uint8_t NUM_BITS = 8;
	// CPU Clock frequency
	static const uint16_t CPU = __AVR_ATmega328P__;
	// sram (bytes)
	static const unsigned long SRAM_SIZE = 2000;
	// eeprom (bytes)
	static const unsigned long EEPROM_SIZE = 1000;
	// flash (bytes)
	static const unsigned long FLASH_SIZE = 32000;

	// Arduino ZERO
#elif defined(__AVR_ATSAMD21G18A__)
	static const uint8_t BOARD = 0x02
								 static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = __AVR_ATSAMD21G18A__;
	static const unsigned long SRAM_SIZE = 32000;
	static const unsigned long EEPROM_SIZE = 16000;
	static const unsigned long FLASH_SIZE = 256000;

	// Arduino DUE
#elif defined(__AVR_ATSAM3X8E__)
	static const uint8_t BOARD = 0x03;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = __AVR_ATSAMD21G18A__;
	static const unsigned long SRAM_SIZE = 96000;
	static const unsigned long EEPROM_SIZE = 0;
	static const unsigned long FLASH_SIZE = 512000;

	// Arduino Leonardo, Micro, Esplora, Yun 16Mhz
#elif defined(__AVR_Atmega32U4__)
	static const uint8_t BOARD = 0x04;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = __AVR_Atmega32U4__;
	static const unsigned long SRAM_SIZE = 2500;
	static const unsigned long EEPROM_SIZE = 1000;
	static const unsigned long FLASH_SIZE = 32000;

	// Arduino Yun 400Mhz
#elif defined(_AVR_AR9331__)
	static const uint8_t BOARD = 0x05;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = _AVR_AR9331__;
	static const unsigned long SRAM_SIZE = 64000000;
	static const unsigned long EEPROM_SIZE = 0;
	static const unsigned long FLASH_SIZE = 16000000;

	// Arduino Leonardo
#elif defined(__AVR_ATmega16U4__)
	static const uint8_t BOARD = 0x06;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = __AVR_ATmega16U4__;
	static const unsigned long SRAM_SIZE = 2560;
	static const unsigned long EEPROM_SIZE = 1000;
	static const unsigned long FLASH_SIZE = 32000;

	// Arduino MEGA/ Arduino MEGA-ADK
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	static const uint8_t BOARD = 0x07;
	static const uint8_t NUM_BITS = 8;
#if defined(__AVR_ATmega1280__)
	static const uint16_t CPU = __AVR_ATmega1280__;
#else
	static const uint16_t CPU = __AVR_ATmega2560__;
#endif
	static const unsigned long SRAM_SIZE = 8000;
	static const unsigned long EEPROM_SIZE = 4000;
	static const unsigned long FLASH_SIZE = 256000;

	// Arduino NANO v3.0+ / Arduino PRO / Arduino PRO 328 / Arduino Ethernet
#elif defined(_AVR_ATmega328__)
	static const uint8_t BOARD = 0x08;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = _AVR_ATmega328__;
	static const unsigned long SRAM_SIZE = 2000;
	static const unsigned long EEPROM_SIZE = 1000;
	static const unsigned long FLASH_SIZE = 32000;


	// Arduino NANO up to v3.0 / Arduino UNO / Arduino PRO
#elif defined(_AVR_ATmega168__)
	static const uint8_t BOARD = 0x09;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = _AVR_ATmega168;
	static const unsigned long SRAM_SIZE = 1000;
	static const unsigned long EEPROM_SIZE = 500;
	static const unsigned long FLASH_SIZE = 16000;

	// Arduino LilyPad 1
#elif defined(_AVR_ATmega168V__)
	static const uint8_t BOARD = 0x0a;
	static const uint8_t CPU = _AVR_ATmega168V__;
	static const unsigned int NUM_BITS = 8;
	static const unsigned long SRAM_SIZE = 1000;
	static const unsigned long EEPROM_SIZE = 500;
	static const unsigned long FLASH_SIZE = 14000;

	// Arduino LilyPad 2
#elif defined(_AVR_ATmega328V__)
	static const uint8_t BOARD = 0x0b;
	static const uint8_t CPU = _AVR_ATmega328V__;
	static const unsigned int NUM_BITS = 8;
	static const unsigned long SRAM_SIZE = 1000;
	static const unsigned long EEPROM_SIZE = 500;
	static const unsigned long FLASH_SIZE = 14000;

	// Arduino Trinket
#elif defined(_AVR_ATTiny85__)
	static const uint8_t BOARD = 0x0c;
	static const uint8_t NUM_BITS = 8;
	static const uint16_t CPU = _AVR_ATTiny85__;
	static const unsigned long SRAM_SIZE = 500;
	static const unsigned long EEPROM_SIZE = 500;
	static const unsigned long FLASH_SIZE = 2500;

	// Arduino Intel Curie/101
#elif  defined(__AVR_ARCv2EM__) || (__CURIE_FACTORY_DATA_H_)
	static const uint8_t BOARD = 0x0d;
	static const uint8_t NUM_BITS = 32;

#if defined(__AVR_ARCv2EM__)
	static const uint16_t CPU = __AVR_ARCv2EM__;
#else
	static const uint16_t CPU = __CURIE_FACTORY_DATA_H_;
#endif

	static const unsigned long SRAM_SIZE = 24000; // might be 80k?
	static const unsigned long EEPROM_SIZE = 0;
	static const unsigned long FLASH_SIZE = 384000;
#else
	static const uint8_t BOARD = 0x00;
	static const uint8_t NUM_BITS = 0;
	static const uint16_t CPU = 0;
	static const unsigned long SRAM_SIZE = 0;
	static const unsigned long EEPROM_SIZE = 0;
	static const unsigned long FLASH_SIZE = 0;
#endif

	BoardInfo();

	bool featureExists(uint8_t feature);
};

#endif // BOARDINFO_HPP
