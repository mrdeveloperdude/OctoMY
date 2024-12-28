#include "ErrorCorrection.hpp"

#include <QDebug>

bool ErrorCorrection::fail(const QString &reason) {
	mErrorString = reason;
	return !mErrorString.isNull() && !mErrorString.isEmpty();
}


QString ErrorCorrection::errorString(){
	return mErrorString;
}


ErrorCorrection::ErrorCorrection(int degree) {
	if (degree < 1 || degree > 255){
		fail("Degree out of range");
		return;
	}
	
	// Start with the monomial x^0
	mCoefficients.resize(degree);
	mCoefficients[degree - 1] = 1;
	
	// Compute the product polynomial (x - r^0) * (x - r^1) * ... * (x - r^{degree-1}),
	// drop the highest term, and store the rest of the coefficients in order of descending powers.
	int root = 1;
	for (int i = 0; i < degree; i++) {
		// Multiply the current product by (x - r^i)
		for (int j = 0; j < mCoefficients.size(); j++) {
			mCoefficients[j] = multiply(mCoefficients[j], static_cast<uint8_t>(root));
			if (j + 1 < mCoefficients.size())
				mCoefficients[j] ^= mCoefficients[j + 1];
		}
		root = (root << 1) ^ ((root >> 7) * 0x11D);  // Multiply by 0x02 mod GF(2^8/0x11D)
	}
}

QByteArray ErrorCorrection::getRemainder(const QByteArray &data) const {
	// Compute the remainder by performing polynomial division
	QByteArray result(mCoefficients.size(), 0);
	for (int i = 0; i < data.size(); i++) {
		uint8_t factor = static_cast<uint8_t>(data.at(i) ^ result.at(0));
		result.remove(0, 1);
		result.append(static_cast<char>(0));
		for (int j = 0; j < result.size(); j++) {
			result[j] ^= multiply(mCoefficients[j], factor);
		}
	}
	return result;
}

uint8_t ErrorCorrection::multiply(uint8_t x, uint8_t y) {
	// Russian peasant multiplication
	int z = 0;
	for (int i = 7; i >= 0; i--) {
		z = (z << 1) ^ ((z >> 7) * 0x11D);
		z ^= ((y >> i) & 1) * x;
	}
	if (z >> 8 != 0) {
		return 0;
	}
	return static_cast<uint8_t>(z);
}



//////////////////////////////////





Ecc ECC_LOW     (0, 1);
Ecc ECC_MEDIUM  (1, 0);
Ecc ECC_QUARTILE(2, 3);
Ecc ECC_HIGH    (3, 2);



const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4][41] = {
	// Version: (note that index 0 is for padding, and is set to an illegal value)
	//0,  1,  2,  3,  4,  5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40    Error correction level
	{-1,  7, 10, 15, 20, 26,  36,  40,  48,  60,  72,  80,  96, 104, 120, 132, 144, 168, 180, 196, 224, 224, 252, 270, 300,  312,  336,  360,  390,  420,  450,  480,  510,  540,  570,  570,  600,  630,  660,  720,  750},  // Low
	{-1, 10, 16, 26, 36, 48,  64,  72,  88, 110, 130, 150, 176, 198, 216, 240, 280, 308, 338, 364, 416, 442, 476, 504, 560,  588,  644,  700,  728,  784,  812,  868,  924,  980, 1036, 1064, 1120, 1204, 1260, 1316, 1372},  // Medium
	{-1, 13, 22, 36, 52, 72,  96, 108, 132, 160, 192, 224, 260, 288, 320, 360, 408, 448, 504, 546, 600, 644, 690, 750, 810,  870,  952, 1020, 1050, 1140, 1200, 1290, 1350, 1440, 1530, 1590, 1680, 1770, 1860, 1950, 2040},  // Quartile
	{-1, 17, 28, 44, 64, 88, 112, 130, 156, 192, 224, 264, 308, 352, 384, 432, 480, 532, 588, 650, 700, 750, 816, 900, 960, 1050, 1110, 1200, 1260, 1350, 1440, 1530, 1620, 1710, 1800, 1890, 1980, 2100, 2220, 2310, 2430},  // High
};

const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
	// Version: (note that index 0 is for padding, and is set to an illegal value)
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
};


