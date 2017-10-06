#include "DiscoveryRole.hpp"

const QDebug &operator<<(QDebug &d, const DiscoveryRole &role){
	switch(role){
		case(ROLE_AGENT):{d.nospace() << "ROLE_AGENT";}break;
		case(ROLE_CONTROL):{d.nospace() << "ROLE_AGENT";}break;
		default: d.nospace() << "ROLE_UNKNOWN";
	}
	return d.maybeSpace();
}


QString DiscoveryRoleToString(DiscoveryRole s){
#define  DiscoveryRoleToStringCASE(A) case (A):return #A
	switch(s){
		default:return "UNKNOWN";
			DiscoveryRoleToStringCASE(ROLE_AGENT);
			DiscoveryRoleToStringCASE(ROLE_CONTROL);
			DiscoveryRoleToStringCASE(ROLE_UNKNOWN);
	}
#undef DiscoveryRoleToStringCASE
}



DiscoveryRole DiscoveryRoleFromString(QString s){
	if("ROLE_AGENT"==s)return ROLE_AGENT;
	if("ROLE_CONTROL"==s)return ROLE_CONTROL;
	return ROLE_UNKNOWN;
}



