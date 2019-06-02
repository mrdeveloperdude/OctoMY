#include "ExpLUT.hpp"
#include <QtMath>

#include <QDebug>

ExpLUT::ExpLUT()
{
	generate();
}

void ExpLUT::generate()
{
	const quint8 sz=sizeof lut;
	const float szf=sz;
	const float fac=5.0;
	const float min=qExp(0.0);
	const float max=qExp(fac);
	const float range=max-min;
	for(quint8 i=0; i<sz; ++i) {
		const float fi=(static_cast<float>(i)*fac)/szf;
		lut[i]=static_cast<quint8>((0xFF*(qExp(fi)-min))/range);
		//qDebug()<<"i="<<i<<", fi="<<fi<<", exp(fi)="<<qExp(fi)<<", sz= "<<sz<<", szf= "<<szf<<", max= "<<max<<", lut[i]= "<<lut[i];
	}

}

quint8 ExpLUT::operator[](const quint8 index)
{
	return lut[index];}
