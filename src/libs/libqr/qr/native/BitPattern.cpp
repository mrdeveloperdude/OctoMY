#include "BitPattern.hpp"


static const int PENALTY_N1 = 3;
static const int PENALTY_N2 = 3;
static const int PENALTY_N3 = 40;
static const int PENALTY_N4 = 10;

void BitPattern::setWidth(int width){
	this->mWidth = width;
	QBitArray::clear();
	width = width > 0 ? width : 0;
	QBitArray::resize(width * width);
}


int BitPattern::width() const{
	return mWidth;
}


void BitPattern::setPixel(int x, int y, bool value){
	if(x>=0 && x < mWidth && y>=0 && y < mWidth){
		setBit(y*mWidth+x, value);
	}
}


bool BitPattern::getPixel(int x, int y) const{
	if(x>=0 && x < mWidth && y>=0 && y < mWidth){
		return at(y*mWidth+x);
	}
	return false;
}

void BitPattern::clear(){
	resize(0);
}




int BitPattern::penaltyScore() const {
	int result = 0;
	// Adjacent modules in row having same color
	for (int y = 0; y < mWidth; y++) {
		bool colorX = getPixel(0, y);
		for (int x = 1, runX = 1; x < mWidth; x++) {
			if (getPixel(x, y) != colorX) {
				colorX = getPixel(x, y);
				runX = 1;
			} else {
				runX++;
				if (runX == 5)
					result += PENALTY_N1;
				else if (runX > 5)
					result++;
			}
		}
	}
	// Adjacent modules in column having same color
	for (int x = 0; x < mWidth; x++) {
		bool colorY = getPixel(x, 0);
		for (int y = 1, runY = 1; y < mWidth; y++) {
			if (getPixel(x, y) != colorY) {
				colorY = getPixel(x, y);
				runY = 1;
			} else {
				runY++;
				if (runY == 5)
					result += PENALTY_N1;
				else if (runY > 5)
					result++;
			}
		}
	}
	
	// 2*2 blocks of modules having same color
	for (int y = 0; y < mWidth - 1; y++) {
		for (int x = 0; x < mWidth - 1; x++) {
			bool  color = getPixel(x, y);
			if (  color == getPixel(x + 1, y) &&
				color == getPixel(x, y + 1) &&
				color == getPixel(x + 1, y + 1))
				result += PENALTY_N2;
		}
	}
	
	// Finder-like pattern in rows
	for (int y = 0; y < mWidth; y++) {
		for (int x = 0, bits = 0; x < mWidth; x++) {
			bits = ((bits << 1) & 0x7FF) | (getPixel(x, y) ? 1 : 0);
			if (x >= 10 && (bits == 0x05D || bits == 0x5D0))  // Needs 11 bits accumulated
				result += PENALTY_N3;
		}
	}
	// Finder-like pattern in columns
	for (int x = 0; x < mWidth; x++) {
		for (int y = 0, bits = 0; y < mWidth; y++) {
			bits = ((bits << 1) & 0x7FF) | (getPixel(x, y) ? 1 : 0);
			if (y >= 10 && (bits == 0x05D || bits == 0x5D0))  // Needs 11 bits accumulated
				result += PENALTY_N3;
		}
	}
	
	// Balance of black and white modules
	int black = 0;
	for (int y = 0; y < mWidth; y++) {
		for (int x = 0; x < mWidth; x++) {
			if (getPixel(x, y))
				black++;
		}
	}
	int total = mWidth * mWidth;
	// Find smallest k such that (45-5k)% <= dark/total <= (55+5k)%
	for (int k = 0; black*20 < (9-k)*total || black*20 > (11+k)*total; k++)
		result += PENALTY_N4;
	return result;
}


