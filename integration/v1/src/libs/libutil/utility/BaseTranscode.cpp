#include "utility/BaseTranscode.hpp"

#include "Standard.hpp"
#include "InfInt.hpp"

//#include <cstring>
#include <QStringBuilder>
#include <QDebug>


QString BaseTranscode::transcode(const QString  &  inalphOrig,  const QString  &  outalph, QByteArray input, bool caseSensitive){
	QString inalph=caseSensitive?inalphOrig:inalphOrig.toLower();

	const unsigned int inbase=inalph.size();
	const unsigned int outbase=outalph.size();
	QString ret;
	InfInt acc=0;
	InfInt base=1;
	for(QByteArray::iterator it=input.begin(),end=input.end();it!=end;++it){
		unsigned char num=*it;
		acc+=base*num;
		base*=inbase;
	}
	while (acc > 0) {
		unsigned int remainder = (acc % outbase).toInt();
		acc /= outbase;
		ret=QString(outalph.at(remainder)) % ret;
	}
	return ret;
}

