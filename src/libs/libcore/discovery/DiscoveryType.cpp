#include "DiscoveryType.hpp"




const QDebug &operator<<(QDebug &d, const DiscoveryType &type){
	switch(type){
		case(TYPE_AGENT):{d.nospace() << "TYPE_AGENT";}break;
		case(TYPE_REMOTE):{d.nospace() << "TYPE_REMOTE";}break;
		case(TYPE_HUB):{d.nospace() << "TYPE_HUB";}break;
		case(TYPE_ZOO):{d.nospace() << "TYPE_ZOO";}break;
		default: d.nospace() << "TYPE_UNKNOWN";
	}
	return d.maybeSpace();
}


QString DiscoveryTypeToString(DiscoveryType s){
#define  DiscoveryTypeToStringCASE(A) case (A):return #A
	switch(s){
		default:return "UNKNOWN";
			DiscoveryTypeToStringCASE(TYPE_AGENT);
			DiscoveryTypeToStringCASE(TYPE_REMOTE);
			DiscoveryTypeToStringCASE(TYPE_HUB);
			DiscoveryTypeToStringCASE(TYPE_ZOO);
			DiscoveryTypeToStringCASE(TYPE_UNKNOWN);
	}
#undef DiscoveryTypeToStringCASE
}



DiscoveryType DiscoveryTypeFromString(QString s){
	if("TYPE_AGENT"==s)return TYPE_AGENT;
	if("TYPE_REMOTE"==s)return TYPE_REMOTE;
	if("TYPE_HUB"==s)return TYPE_HUB;
	if("TYPE_ZOO"==s)return TYPE_ZOO;
	return TYPE_UNKNOWN;
}


