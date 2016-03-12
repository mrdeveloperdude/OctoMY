#include "basic/Standard.hpp"


QDebug operator<< (QDebug d, void *p){
	WWFUNCTIONGATE();
	d.nospace() << QString::number((long long)p, 16);
	return d.space();
}


const QString operator+ ( const QString &s, void *p ){
	WWFUNCTIONGATE();
	return (s+ QString::number((long long)p, 16));
}
