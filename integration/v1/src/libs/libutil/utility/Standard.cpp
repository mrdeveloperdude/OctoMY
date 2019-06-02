#include "utility/Standard.hpp"


QDebug operator<< (QDebug d, void *p){
	OC_FUNCTIONGATE();
	d.nospace() << QString::number((long long)p, 16);
	return d.space();
}


const QString operator+ ( const QString &s, void *p ){
	OC_FUNCTIONGATE();
	return (s+ QString::number((long long)p, 16));
}
