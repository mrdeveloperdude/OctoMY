#include "NodeRole.hpp"

const QDebug &operator<<(QDebug &d, const NodeRole &role){
	switch(role){
		case(ROLE_AGENT):{d.nospace() << "ROLE_AGENT";}break;
		case(ROLE_CONTROL):{d.nospace() << "ROLE_AGENT";}break;
		default: d.nospace() << "ROLE_UNKNOWN";
	}
	return d.maybeSpace();
}


QString nodeRoleToString(NodeRole s){
#define  NodeRoleToStringCASE(A) case (A):return #A
	switch(s){
		default:return "UNKNOWN";
			NodeRoleToStringCASE(ROLE_AGENT);
			NodeRoleToStringCASE(ROLE_CONTROL);
			NodeRoleToStringCASE(ROLE_UNKNOWN);
	}
#undef NodeRoleToStringCASE
}



NodeRole nodeRoleFromString(QString s){
	if("ROLE_AGENT"==s)return ROLE_AGENT;
	if("ROLE_CONTROL"==s)return ROLE_CONTROL;
	return ROLE_UNKNOWN;
}



