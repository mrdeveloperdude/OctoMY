#ifndef BITPATTERN_HPP
#define BITPATTERN_HPP

#include <QBitArray>


class BitPattern: public QBitArray{
private:
	int mWidth{0};
public:
	void setWidth(int width);
	int width() const;
	void setPixel(int x, int y, bool value);
	bool getPixel(int x, int y) const;
	void clear();
	int penaltyScore() const;
};




#endif // BITPATTERN_HPP

