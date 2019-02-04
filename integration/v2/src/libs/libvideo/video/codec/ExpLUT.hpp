#ifndef EXPLUT_HPP
#define EXPLUT_HPP


#include <QtGlobal>

class ExpLUT
{
private:
	quint8 lut[0xFF];
public:
	ExpLUT();
private:
	void generate();

public:
	quint8 operator[](const quint8 index);
};

#endif // EXPLUT_HPP
