#include "NodeRole.hpp"

#include "app/Constants.hpp"

const QDebug &operator<<(QDebug &d, const NodeRole &role)
{
	switch(role) {
	case(ROLE_AGENT): {
		d.nospace() << Constants::OC_NODE_ROLE_AGENT_NAME;
	}
	break;
	case(ROLE_CONTROL): {
		d.nospace() << Constants::OC_NODE_ROLE_CONTROL_NAME;
	}
	break;
	default:
		d.nospace() << Constants::OC_NODE_ROLE_UNKNOWN_NAME;
	}
	return d.maybeSpace();
}


QString nodeRoleToString(NodeRole s)
{
	switch(s) {
	case(ROLE_AGENT):
		return Constants::OC_NODE_ROLE_AGENT_NAME;
	case(ROLE_CONTROL):
		return Constants::OC_NODE_ROLE_CONTROL_NAME;
	default:
		return Constants::OC_NODE_ROLE_UNKNOWN_NAME;
	}
}



NodeRole nodeRoleFromString(QString s)
{
	if(Constants::OC_NODE_ROLE_AGENT_NAME==s) {
		return ROLE_AGENT;
	}
	if(Constants::OC_NODE_ROLE_CONTROL_NAME==s) {
		return ROLE_CONTROL;
	}
	return ROLE_UNKNOWN;
}



