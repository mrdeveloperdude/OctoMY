#include "Helpers.hpp"

#include <QDebug>

#include "ErrorCorrection.hpp"


int numRawDataModules(int ver) {
	if (ver < 1 || ver > 40){
		qDebug() << "Version number out of range";
		return -1;
	}
	int result = (16 * ver + 128) * ver + 64;
	if (ver >= 2) {
		int numAlign = ver / 7 + 2;
		result -= (25 * numAlign - 10) * numAlign - 55;
		if (ver >= 7){
			result -= 18 * 2;
		}
	}
	return result;
}




int numDataCodewords(int ver, const Ecc &ecl) {
	if (ver < 1 || ver > 40){
		qDebug() << "Version number out of range";
	}
	return numRawDataModules(ver) / 8 - NUM_ERROR_CORRECTION_CODEWORDS[ecl.ordinal][ver];
}



// Increase the error correction level while the data still fits in the current version number
Ecc *calculateBoostedEcc(const int version, const int dataUsedBits, Ecc *ecl){
	Ecc *newEcl = ecl;
	if (dataUsedBits <= numDataCodewords(version, ECC_MEDIUM  ) * 8)  newEcl = &ECC_MEDIUM  ;
	if (dataUsedBits <= numDataCodewords(version, ECC_QUARTILE) * 8)  newEcl = &ECC_QUARTILE;
	if (dataUsedBits <= numDataCodewords(version, ECC_HIGH    ) * 8)  newEcl = &ECC_HIGH    ;
	return newEcl;
}




std::vector<int> alignmentPatternPositions(int ver) {
	if (ver < 1 || ver > 40){
		qDebug() << "Version number out of range";
		return std::vector<int>();
	}
	else if (ver == 1){
		return std::vector<int>();
	}
	else {
		int numAlign = ver / 7 + 2;
		int step;
		if (ver != 32)
			step = (ver * 4 + numAlign * 2 + 1) / (2 * numAlign - 2) * 2;
		else
			step = 26;
		std::vector<int> result;
		int size = ver * 4 + 17;
		for (int i = 0, pos = size - 7; i < numAlign - 1; i++, pos -= step){
			result.insert(result.begin(), pos);
		}
		result.insert(result.begin(), 6);
		return result;
	}
}

