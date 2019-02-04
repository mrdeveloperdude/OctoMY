#include "BaseTranscode.hpp"


#include "utility/math/InfInt.hpp"

#include "uptime/MethodGate.hpp"

//#include <cstring>
#include <QStringBuilder>
#include <QDebug>


QString BaseTranscode::transcode(const QString  &  inalphOrig,  const QString  &  outalph, QByteArray input, bool caseSensitive)
{
	OC_FUNCTIONGATE();
	QString inalph=caseSensitive?inalphOrig:inalphOrig.toLower();

	const int inbase=inalph.size();
	const int outbase=outalph.size();
	QString ret;
	InfInt acc=0;
	InfInt base=1;
	for(QByteArray::iterator it=input.begin(), end=input.end(); it!=end; ++it) {
		unsigned char num=static_cast<unsigned char>(*it);
		acc+=base*num;
		base*=inbase;
	}
	while (acc > 0) {
		const int remainder = (acc % outbase).toInt();
		acc /= outbase;
		ret=QString(outalph.at(remainder)) % ret;
	}
	return ret;
}

