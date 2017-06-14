#ifndef CONVERTER_HPP
#define CONVERTER_HPP
#include "ArduMY.hpp"

union Converter {
	int8_t int8[8];
	uint8_t uint8[8];
	int16_t int16[4];
	uint16_t uint16[4];
	int32_t int32[2];
	uint32_t uint32[2];
	int64_t int64;
	uint64_t uint64;
	float float32[2];
	double float64;


	Converter():
		uint8{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } {

	}
};


#endif // CONVERTER_HPP
