#ifndef CONVERTER_HPP
#define CONVERTER_HPP

union Converter {
	char int8[4];
	unsigned char uint8[4];
	short int16[2];
	unsigned short uint16[2];
	int int32;
	unsigned int uint32;
	float float32;
	double float64;
};


#endif // CONVERTER_HPP
