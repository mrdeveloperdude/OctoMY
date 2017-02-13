#include "BoardInfo.hpp"


BoardInfo::BoardInfo() {
  memset(FEATURES, false, NUM_FEATURES);

  static const unsigned long BOARD_UNKNOWN = 0x0;
  static const unsigned long BOARD_UNO = 0x01;
  static const unsigned long BOARD_ZERO = 0x02;
  static const unsigned long BOARD_DUE = 0x03;
  static const unsigned long BOARD_MICRO= 0x04;
  static const unsigned long BOARD_YUN_400 = 0x05;
  static const unsigned long BOARD_LEONARDO = 0x06;
  static const unsigned long BOARD_MEGA = 0x07;
  static const unsigned long BOARD_NANO = 0x08;
  static const unsigned long BOARD_NANO_3 = 0x09;
  static const unsigned long BOARD_LILYPAD = 0x08;
  static const unsigned long BOARD_TRINKET = 0x09;


  switch (BoardInfo::BOARD) {
	case BoardInfo::BOARD_UNO:
	  strcpy(BOARD_NAME, "UNO");
	  strcpy(CPU_NAME, "ATmega328P");
	break;
	case BoardInfo::BOARD_ZERO:
	  strcpy(BOARD_NAME, "Zero");
	  strcpy(CPU_NAME, "ATSAMD21G18A");
	  FEATURES[BoardInfo::FEATURE_ANALOG_OUT] = true;
	break;
	case BoardInfo::BOARD_DUE:
	  strcpy(BOARD_NAME, "Due");
	  strcpy(CPU_NAME, "ATSAM3X8E");
	  FEATURES[BoardInfo::FEATURE_ANALOG_OUT] = true;
	break;
	case BoardInfo::BOARD_MICRO:
	  strcpy(BOARD_NAME, "Micro");
	  strcpy(CPU_NAME, "Atmega32U4");
	break;
	case BoardInfo::BOARD_YUN_400:
	  strcpy(BOARD_NAME, "Yun");
	  strcpy(CPU_NAME, "AR9331");
	break;
	case BoardInfo::BOARD_LEONARDO:
	  strcpy(BOARD_NAME, "Leonardo");
	  strcpy(CPU_NAME, "ATmega16U4");
	break;
	case BoardInfo::BOARD_MEGA:
	  strcpy(BOARD_NAME, "Mega");
	  strcpy(CPU_NAME, "ATmega1280");
	  FEATURES[BoardInfo::FEATURE_MULTIPLE_SERIAL] = true;
	break;
	case BoardInfo::BOARD_NANO:
	  strcpy(BOARD_NAME, "Nano");
	  strcpy(CPU_NAME, "ATmega168");
	break;
	case BoardInfo::BOARD_NANO_3:
	  strcpy(BOARD_NAME, "Nano");
	  strcpy(CPU_NAME, "ATmega328");
	break;
	case BoardInfo::BOARD_LILYPAD:
	  strcpy(BOARD_NAME, "Lilypad");
	  strcpy(CPU_NAME, "ATmega168V");
	break;
	case BoardInfo::BOARD_LILYPAD_2:
	  strcpy(BOARD_NAME, "Lilypad");
	  strcpy(CPU_NAME, "ATmega328V");
	break;
	case BoardInfo::BOARD_TRINKET:
	  strcpy(BOARD_NAME, "Trinket");
	  strcpy(CPU_NAME, "ATTiny85");
	break;
	case BoardInfo::BOARD_101:
	  strcpy(BOARD_NAME, "101");
	  strcpy(CPU_NAME, "ARCv2EM");
	  FEATURES[BoardInfo::FEATURE_BLUETOOTH_4] = true;
	  FEATURES[BoardInfo::FEATURE_ACCELEROMETER] = true;
	  FEATURES[BoardInfo::FEATURE_GYROSCOPE] = true;
	break;
	default:
	  strcpy(BOARD_NAME, "Unknown");
	  strcpy(CPU_NAME, "Unknown");

  }

}


bool BoardInfo::featureExists(uint8_t feature) {
  return  ((feature < BoardInfo::NUM_FEATURES) && (BoardInfo::FEATURES[feature]));
}
